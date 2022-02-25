using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Runtime.InteropServices;
using System.Timers;
using System.Windows.Threading;

namespace xwLedConfigurator {

    public class Download {

        public delegate void downloadEvent_t(eventType type, double eventData);
        public event downloadEvent_t downloadEvent;

        public enum eventType {
            ERROR_NO_RESPONSE,
            PROGRESS_UPDATE,
            FINISHED
        }

        class outputController {
            public int output;
            public int colorChannel;
            public channel_t channel;
            public List<byte> outputData;

            public outputController(channel_t channel, int output, int colorChannel) {
                this.channel = channel;
                this.output = output;
                this.colorChannel = colorChannel;

                //calculate output data
                //prepare variables
                double currentTime = 0;
                outputData = new List<byte>();
                bool continueLoop = true;
                int currentObject = 0;
                if (channel.ledObjects.Count == 0) continueLoop = false;

                //loop for all objects
                while (continueLoop) {
                    ledObject led = channel.ledObjects[currentObject];
                    if (currentTime == led.starttime) {
                        outputData.AddRange(led.getBuffer(colorChannel, false));
                        currentTime += led.length;
                        currentObject++;
                        if (currentObject == channel.ledObjects.Count) {
                            //add final 1ms long sob if needed
                            if (channel.eolOption == 3) {
                                byte[] blankFrame = new byte[12];
                                blankFrame[0] = 0x01;
                                blankFrame[2] = 0x01;
                                blankFrame[11] = 1; //marker, so empty space can be reconstructed when uploaded
                                outputData.AddRange(blankFrame); 
                            }
                            continueLoop = false;
                        }
                        continue;
                    }
                    else {
                        uint blankTime = (uint)(led.starttime - currentTime);
                        byte[] blankFrame = new byte[12];
                        blankFrame[0] = 0x01;
                        blankFrame[2] = (byte)(blankTime);
                        blankFrame[3] = (byte)(blankTime >> 8);
                        blankFrame[11] = 1; //marker, so empty space can be reconstructed when uploaded
                        outputData.AddRange(blankFrame);
                        currentTime += blankTime;
                    }
                }

                //add end of line object
                byte[] eod = new byte[12];
                eod[0] = 0xF0;
                if (channel.eolOption == 0) eod[1] = 1; //restart output
                if (channel.eolOption == 1) eod[1] = 2; //reverse output
                if (channel.eolOption == 2) eod[1] = 0; //keep output unchanged at last brightness
                if (channel.eolOption == 3) eod[1] = 0; //keep output unchanged at 0 brightness (blank frame was inserted before)
                outputData.AddRange(eod);
            }            
        }

        enum downloadState_t {
            STATE_INIT,
            STATE_GET_FRAME_SIZE,
            STATE_RESET_POINTERS,
            STATE_ERASE_DEVICE,
            STATE_DOWNLOAD,
            STATE_REINITIALIZE,
            STATE_FINISHED,
            STATE_ERROR
        }
        downloadState_t downloadState;
        List<sequence_t> sequenceList;
        List<byte> fullConfig;
        int maxFrameSize;
        int bytesLoaded;
        DispatcherTimer timeoutTimer;

        public Download() {                        
        }

        public void startDownload(List<sequence_t> sequenceList) {
            
            //init variables
            this.sequenceList = sequenceList;
            downloadState = downloadState_t.STATE_INIT;

            //prepare the data buffer
            fullConfig = new List<byte>();

            //prepare header            
            fullConfig.AddRange(get32BitBuffer(0));     // length will be changed later
            fullConfig.Add((byte)sequenceList.Count);   // Number of sequencec
            fullConfig.Add(0);                          // Unused
            fullConfig.Add((byte)xwCom.CONFIG_TYPE);    // Config Type, 1 for xwLedControl
            fullConfig.Add((byte)xwCom.CONFIG_VERSION); // Config Version, 1 for the current xwLedControl FW
            fullConfig.AddRange(new byte[16]);          // 16 unused bytes

            //add a space to put the offset to each sequence header later
            int sequenceTableAddress = fullConfig.Count;
            for (int i = 0; i < sequenceList.Count; i++) fullConfig.AddRange(get32BitBuffer(0));

            //put all sequence buffers and set the offsets (to config base) in the table
            foreach (sequence_t sequence in sequenceList) {
                int sequenceAddress = fullConfig.Count;             
                bufferChange(ref fullConfig, sequenceTableAddress, get32BitBuffer((uint)sequenceAddress));
                sequenceTableAddress += 4;
                fullConfig.AddRange(getSequenceBuffer(sequence, sequenceAddress));                
            }

            //put config length
            int configLength = fullConfig.Count;
            bufferChange(ref fullConfig, 0, get32BitBuffer((uint)configLength));

            //calculate crc and add at the end
            ushort crc = CRC.calc(fullConfig.ToArray());
            fullConfig.AddRange(get16BitBuffer(crc));

            //disable connection timout control
            Connection.refreshControl = false;

            //start download
            downloadState = downloadState_t.STATE_GET_FRAME_SIZE;
            Connection.putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.GET_FRAME_MAX_SIZE });

            //start timeout
            setTimeout(100);
        }

        byte[] getSequenceBuffer(sequence_t sequence, int sequenceOffset) {

            // Prepare header
            List<byte> sequenceConfig = new List<byte>();

            // Add name
            byte[] sequenceName = Encoding.ASCII.GetBytes(sequence.name);
            Array.Resize(ref sequenceName, 12);
            sequenceConfig.AddRange(sequenceName);

            // Create output controllers for each output in sequence
            // Also, prepare buffer to reconstruct sw channels when uploaded from device
            List<outputController> outputControllers = new List<outputController>();
            List<byte> reconstructBuffer = new List<byte>();
            int numOutputs = 0;
            foreach (channel_t channel in sequence.channels) {
                if (channel.isRGB) {
                    numOutputs += 3;
                    outputControllers.Add(new outputController(channel, channel.outputs[0].assignment, 1));
                    outputControllers.Add(new outputController(channel, channel.outputs[1].assignment, 2));
                    outputControllers.Add(new outputController(channel, channel.outputs[2].assignment, 3));
                    reconstructBuffer.AddRange(new byte[] { 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3 });
                }
                else {
                    numOutputs += 1;
                    outputControllers.Add(new outputController(channel, channel.outputs[0].assignment, 0));
                    reconstructBuffer.AddRange(new byte[] { channel.color.R, channel.color.G, channel.color.B, 0 });
                }
            }

            // Add data
            sequenceConfig.Add((byte)numOutputs);                           // Number of channles 
            sequenceConfig.Add((byte)sequence.dimInfo);                     // Dim Info
            sequenceConfig.AddRange(new byte[2]);                           // Unused
            sequenceConfig.AddRange(get32BitBuffer(sequence.speedInfo));    // Speed Info
            sequenceConfig.AddRange(new byte[8]);                           // Unused

            //add a space to put the offset to each definition table header later
            int outputTableAddress = sequenceConfig.Count;
            foreach (outputController controller in outputControllers) sequenceConfig.AddRange(get32BitBuffer(0));

            //put all output assignments here
            foreach (outputController controller in outputControllers) sequenceConfig.Add((byte)controller.output);

            //make sure array size can be divided by 4
            int fill = sequenceConfig.Count % 4;
            if (fill > 0) sequenceConfig.AddRange(new byte[4-fill]);

            //here, add 4 bytes for each output channel, so the SW can reconstruct SW channels and colors when uploaded from the device
            //byte 1 2 and 3 are color channel RGB when a single channel, or 0 for RGB channels. byte 4 is the outputs color assignment, 0 for a single color channel, 1 for Red channel, 2 Green, 3 Blue
            sequenceConfig.AddRange(reconstructBuffer.ToArray());

            //for each output, get the whole buffer, copy it into the config, and adjust the offset (to configBase) in the base table
            foreach (outputController controller in outputControllers) {
                int outputAddress = sequenceConfig.Count + sequenceOffset;             
                bufferChange(ref sequenceConfig, outputTableAddress, get32BitBuffer((uint)outputAddress));
                outputTableAddress += 4;
                sequenceConfig.AddRange(controller.outputData.ToArray());                
            }            

            return sequenceConfig.ToArray();
        }

        public void frameReceiver(ref cRxFrame rxFrame) {

            //When config frame is received
            if (rxFrame.scope == (byte)xwCom.SCOPE.CONFIG) {

                //max frame size response received
                if (rxFrame.data[0] == (byte)xwCom.CONFIG_RESPONSE.RESPONSE_FRAME_MAX_SIZE) {
                    if (downloadState == downloadState_t.STATE_GET_FRAME_SIZE) {
                        maxFrameSize = rxFrame.data[1] * 256;
                        maxFrameSize += rxFrame.data[2];
                        setTimeout(3000);
                        downloadState = downloadState_t.STATE_ERASE_DEVICE;
                        Connection.putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.ERASE_CONFIG });
                    }
                }
                //acknowledge frame received
                if (rxFrame.data[0] == (byte)xwCom.CONFIG_RESPONSE.RESPONSE_ACKNOWLEDGE) {
                    if (rxFrame.data[1] == 1) {
                        //device was sucessfully erased, reset pointers next
                        if (downloadState == downloadState_t.STATE_ERASE_DEVICE) {                            
                            setTimeout(100);
                            downloadState = downloadState_t.STATE_RESET_POINTERS;
                            Connection.putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.RESET_POINTER });
                        }
                        //pointers have been resetted, send first data frame now
                        else if (downloadState == downloadState_t.STATE_RESET_POINTERS) {
                            bytesLoaded = 0;
                            setTimeout(500);
                            sendDataFrame();
                            downloadState = downloadState_t.STATE_DOWNLOAD;
                        }
                        //last dataframe was received correct
                        else if (downloadState == downloadState_t.STATE_DOWNLOAD) {
                            //send next framse
                            if (bytesLoaded < fullConfig.Count) {
                                setTimeout(500);
                                sendDataFrame();
                            }
                            //reinit device
                            else {
                                setTimeout(100);
                                downloadState = downloadState_t.STATE_REINITIALIZE;
                                Connection.putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.REINITIALIZE });
                            }                            
                        }
                        //reinit complete, finished
                        else if (downloadState == downloadState_t.STATE_REINITIALIZE) {
                            stopTimeout();
                            Connection.refreshControl = true;
                            if (downloadEvent != null) downloadEvent(eventType.FINISHED, 0);
                        }
                    }
                }
            }
        }

        //send next data frame
        public void sendDataFrame() {
            int frameSize = fullConfig.Count - bytesLoaded;
            if (frameSize > maxFrameSize) frameSize = maxFrameSize;            
            byte[] txframe = new byte[frameSize+2];
            txframe[0] = (byte)xwCom.CONFIG.SET_FRAME;
            txframe[1] = (byte)frameSize;
            for (int i = 0; i < frameSize; i++) txframe[i+2] = fullConfig[i + bytesLoaded];
            bytesLoaded += frameSize;
            double progressIndicator = bytesLoaded / (double)fullConfig.Count;
            if (downloadEvent != null) downloadEvent(eventType.PROGRESS_UPDATE, progressIndicator);
            Connection.putFrame((byte)xwCom.SCOPE.CONFIG, txframe);
        }

        public void bufferChange(ref List<byte> buffer, int offset, byte[] newdata) {
            for (int i = 0; i < newdata.Length; i++) buffer[i + offset] = newdata[i];
        }

        byte[] get32BitBuffer(uint value) {
            byte[] buffer = new byte[4];
            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);
            buffer[2] = (byte)(value >> 16);
            buffer[3] = (byte)(value >> 24);
            return buffer;
        }

        byte[] get16BitBuffer(int value) {
            byte[] buffer = new byte[2];
            buffer[0] = (byte)value;
            buffer[1] = (byte)(value >> 8);
            return buffer;
        }

        void setTimeout(int milliseconds) {
            if (timeoutTimer == null) {
                timeoutTimer = new DispatcherTimer();
                timeoutTimer.Interval = TimeSpan.FromMilliseconds(500);
                timeoutTimer.Tick += timeoutElapsed;
            }
            timeoutTimer.Stop();
            timeoutTimer.Interval = TimeSpan.FromMilliseconds(milliseconds);
            timeoutTimer.Start();
        }

        void stopTimeout() {
            timeoutTimer.Stop();
        }

        void timeoutElapsed(object sender, EventArgs e) {
            downloadState = downloadState_t.STATE_ERROR;
            Connection.refreshControl = true;
            timeoutTimer.Stop();
            if (downloadEvent != null) downloadEvent(eventType.ERROR_NO_RESPONSE, 0);
        }        

    }
}
