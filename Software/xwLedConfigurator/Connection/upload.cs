using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Runtime.InteropServices;
using System.Timers;
using System.Windows.Threading;
using System.Windows.Media;
using System.Reflection;

namespace xwLedConfigurator {

    public class Upload {

        public delegate void uploadEvent_t(eventType type, double eventData);
        public event uploadEvent_t uploadEvent;

        class outputController {
            public int output;
            public Color channelColor = Colors.Black;
            public int colorChannel = 0;
            public List<ledObject> ledObjects;
            public int eodOption;
            public byte dim, minAux, maxAux;

            ledObject getObjectFromBuffer(byte[] buffer) {
                foreach (Type type in Assembly.GetExecutingAssembly().GetTypes()) {                    
                    if (type.BaseType == typeof(ledObject)) {
                        ledObject ledobject = (ledObject) Activator.CreateInstance(type, new object[] {});
                        if (ledobject.applyBuffer(buffer)) return ledobject;                        
                    }                    
                }
                return null;
            }

            public outputController(ref List<byte> config, int headerAddress) {                

                this.output = config[headerAddress];
                this.dim = config[headerAddress + 1];
                this.minAux = config[headerAddress + 2];
                this.maxAux = config[headerAddress + 3];
                colorChannel = config[headerAddress + 7];

                //colorChannel 0 means this output was on a single color channel. save the channel color, not required for RGB channels
                if (colorChannel == 0) {
                    channelColor.R = config[headerAddress + 4];
                    channelColor.G = config[headerAddress + 5];
                    channelColor.B = config[headerAddress + 6];
                }

                //loop through object data buffer until EOD os reached
                ledObjects = new List<ledObject>();
                bool continueLoop = true;
                bool lastBlank = false;
                double currentTime = 0;
                int currentAddress = BitConverter.ToInt32(new byte[] { config[headerAddress + 0x08], config[headerAddress + 0x09], config[headerAddress + 0x0A], config[headerAddress + 0x0B] });

                while (continueLoop) {
                    byte[] objectData = new byte[12];
                    for (int i = 0; i < 12; i++) objectData[i] = config[currentAddress++];
                    
                    //end of line processing
                    if (objectData[0] == 0xF0) {
                        if (objectData[1] == 0) {
                            if (lastBlank) eodOption = 3;
                            else eodOption = 2;
                        }
                        if (objectData[1] == 1) eodOption = 0;
                        if (objectData[1] == 2) eodOption = 1;

                        continueLoop = false;
                        continue;
                    }

                    //any other objects
                    ledObject ledobject = getObjectFromBuffer(objectData);
                    
                    if (ledobject != null) {
                        //special case for sob: if the last byte is set to 1, this marks that we just should include a blank in the timeline
                        if (ledobject is sob) {
                            if (objectData[11] == 1) {
                                currentTime += ledobject.length;
                                continue;
                            }
                        }

                        //for all other objects, adjust starttime and add to the list
                        ledobject.starttime = currentTime;
                        currentTime += ledobject.length;
                        ledObjects.Add(ledobject);
                    }
                }
            }
        }

        public enum eventType {
            ERROR_NO_RESPONSE,
            ERROR_PROCESSING,
            ERROR_DEVICE_EMPTY,
            PROGRESS_UPDATE,
            FINISHED
        }      

        enum uploadState_t {
            STATE_INIT,
            STATE_GET_FRAME_SIZE,
            STATE_RESET_POINTERS,
            STATE_UPLOAD,
            STATE_CRC,
            STATE_PROCESS,
            STATE_ERROR
        }
        uploadState_t uploadState;
        List<sequence_t> sequenceList;
        List<byte> fullConfig;
        int maxFrameSize;
        int bytesLoaded;
        int configSize;
        ushort crcFlash;
        DispatcherTimer timeoutTimer;

        public Upload() {                        
        }

        public void startUpload(List<sequence_t> sequenceList) {

            //init variables
            this.sequenceList = sequenceList;
            fullConfig = new List<byte>();

            //start download
            uploadState = uploadState_t.STATE_GET_FRAME_SIZE;
            Connection.putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.GET_FRAME_MAX_SIZE });

            //start timeout
            setTimeout(100);
        }

        public void processData() {

            //read config header first
            int numSequences = fullConfig[4];
            int configType = fullConfig[6];
            int configVersion = fullConfig[7];
            ushort crc = CRC.calc(fullConfig.ToArray());

            if ((crc != crcFlash) || (configType != xwCom.CONFIG_TYPE) || (configVersion != xwCom.CONFIG_VERSION)) {
                if (uploadEvent != null) uploadEvent(eventType.ERROR_PROCESSING, 0);
                return;
            }

            //here,the readed data is valid, clear old sequences proceed with processing
            sequenceList.Clear();

            //for each sequence
            for (int i = 0; i < numSequences; i++) {
                sequence_t sequence = new sequence_t();
                int sequenceHeaderAddress = BitConverter.ToInt32(new byte[] { fullConfig[0x08 + i * 4], fullConfig[0x08 + i * 4 + 1], fullConfig[0x08 + i * 4 + 2], fullConfig[0x08 + i * 4 + 3] });            
                
                //read sequence header information
                byte[] name = new byte[12];
                Array.Copy(fullConfig.ToArray(), sequenceHeaderAddress, name, 0, 12);
                sequence.name = Encoding.Default.GetString(name);

                int numoutputs = fullConfig[sequenceHeaderAddress + 0x0C];
                sequence.dimInfo = fullConfig[sequenceHeaderAddress + 0x0D];
                sequence.speedInfo = BitConverter.ToUInt32(new byte[] { fullConfig[sequenceHeaderAddress + 0x10], fullConfig[sequenceHeaderAddress + 0x11], fullConfig[sequenceHeaderAddress + 0x12], fullConfig[sequenceHeaderAddress + 0x13] });

                //parse the data from all channels to list of white ledobjects
                List<outputController> outputControllers = new List<outputController>();
                
                for (int j = 0; j < numoutputs; j++) {
                    int channelHeaderAddress = BitConverter.ToInt32(new byte[] { fullConfig[sequenceHeaderAddress + 0x14 + j * 4], fullConfig[sequenceHeaderAddress + 0x14 + j * 4 + 1], fullConfig[sequenceHeaderAddress + 0x14 + j * 4 + 2], fullConfig[sequenceHeaderAddress + 0x14 + j * 4 + 3] });                    
                    outputControllers.Add(new outputController(ref fullConfig, channelHeaderAddress));
                }

                //reconstruct the original sw channels
                int currentOutput = 0;
                while (currentOutput < numoutputs) {
                    outputController controller = outputControllers[currentOutput];

                    //this was a single color channel originally
                    //create a SW channel with all settings, and apply channel color to all led objects
                    if (controller.colorChannel == 0) {
                        channel_t channel = new channel_t();
                        channel.isRGB = false;
                        channel.color = controller.channelColor;
                        channel.eolOption = controller.eodOption;
                        channel.channelDim = controller.dim;
                        channel.channelAuxMin = controller.minAux;
                        channel.channelAuxMax = controller.maxAux;
                        channel.outputs.Add(new output_t());
                        channel.outputs[0].assignment = controller.output;
                        foreach (ledObject ledobject in controller.ledObjects) {
                            ledobject.updateChannelColor(controller.channelColor, false);
                            channel.ledObjects.Add(ledobject);
                        }
                        sequence.channels.Add(channel);
                    }

                    //this was the read channe of an RGB SW channel originally. Green and Blue will follow after
                    //create a SW channel with all settings, take reede green and blue as ask the led objects for color reconstruction
                    if (controller.colorChannel == 1) {
                        outputController greenChannel = outputControllers[currentOutput + 1];
                        outputController blueChannel = outputControllers[currentOutput + 2];
                        channel_t channel = new channel_t();
                        channel.isRGB = true;
                        channel.eolOption = controller.eodOption;
                        channel.channelDim = controller.dim;
                        channel.channelAuxMin = controller.minAux;
                        channel.channelAuxMax = controller.maxAux;
                        channel.outputs.AddRange(new output_t[]{ new output_t(), new output_t(), new output_t() });
                        channel.outputs[0].assignment = controller.output;
                        channel.outputs[1].assignment = greenChannel.output;
                        channel.outputs[2].assignment = blueChannel.output;
                        for (int j=0; j<controller.ledObjects.Count; j++) {
                            ledObject red = controller.ledObjects[j];
                            ledObject green = greenChannel.ledObjects[j];
                            ledObject blue = blueChannel.ledObjects[j];
                            red.reconstructColors(red, green, blue);
                            channel.ledObjects.Add(red);
                        }
                        sequence.channels.Add(channel);
                    }
                    currentOutput++;
                }

                sequenceList.Add(sequence);
            }
           
            if (uploadEvent != null) uploadEvent(eventType.FINISHED, 0);
        }
        
        public void frameReceiver(ref cRxFrame rxFrame) {
            //When config frame is received
            if (rxFrame.scope == (byte)xwCom.SCOPE.CONFIG) {

                //max frame size response received
                if (rxFrame.data[0] == (byte)xwCom.CONFIG_RESPONSE.RESPONSE_FRAME_MAX_SIZE) {
                    if (uploadState == uploadState_t.STATE_GET_FRAME_SIZE) {
                        maxFrameSize = rxFrame.data[1] * 256;
                        maxFrameSize += rxFrame.data[2];
                        setTimeout(100);
                        uploadState = uploadState_t.STATE_RESET_POINTERS;
                        Connection.putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.RESET_POINTER });
                    }
                }

                //acknowledge frame received
                if (rxFrame.data[0] == (byte)xwCom.CONFIG_RESPONSE.RESPONSE_ACKNOWLEDGE) {
                    if (rxFrame.data[1] == 1) {
                        //pointers have been resetted, request first 4 bytes of data now
                        if (uploadState == uploadState_t.STATE_RESET_POINTERS) {
                            bytesLoaded = 0;
                            crcFlash = 0;
                            setTimeout(2000);                            
                            uploadState = uploadState_t.STATE_UPLOAD;
                            Connection.putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.GET_FRAME, (byte) 4 });
                            uploadState = uploadState_t.STATE_UPLOAD;
                        }                        
                    }
                }

                //frame received
                if (rxFrame.data[0] == (byte)xwCom.CONFIG_RESPONSE.RESPONSE_FRAME) {
                    if (uploadState == uploadState_t.STATE_UPLOAD) {
                        //save frame data
                        int framelength = rxFrame.data[1];
                        for (int i = 0; i < framelength; i++) fullConfig.Add(rxFrame.data[i + 2]);

                        //at the first frame, read config length, add 2 to also read crc
                        //if length is 0, the device is empty, raise an error and stop
                        if (bytesLoaded == 0) {
                            configSize = BitConverter.ToInt32(new byte[] { fullConfig[0], fullConfig[1], fullConfig[2], fullConfig[3] });
                            if (configSize == -1) {
                                stopTimeout();
                                uploadState = uploadState_t.STATE_ERROR;
                                if (uploadEvent != null) uploadEvent(eventType.ERROR_DEVICE_EMPTY, 0);
                                return;
                            }
                        }

                        //bookkeeping and visual information
                        bytesLoaded += framelength;
                        double progress = (double)bytesLoaded / (double)configSize;
                        if (uploadEvent != null) uploadEvent(eventType.PROGRESS_UPDATE, progress);

                        //read next frame, or when enough data is loaded, finish and hand read crc
                        int bytesStillToRead = configSize - bytesLoaded;
                        if (bytesStillToRead > 0) {
                            if (bytesStillToRead > maxFrameSize) bytesStillToRead = maxFrameSize;
                            if (bytesStillToRead % 2 == 1) bytesStillToRead++;
                            setTimeout(500);
                            Connection.putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.GET_FRAME, (byte)bytesStillToRead });
                        }
                        else {
                            setTimeout(500);
                            uploadState = uploadState_t.STATE_CRC;
                            Connection.putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.GET_FRAME, 2});                            
                        }
                    }
                    else if (uploadState == uploadState_t.STATE_CRC) {
                        stopTimeout();
                        if (rxFrame.data[1] == 2) {
                            uploadState = uploadState_t.STATE_PROCESS;
                            crcFlash = BitConverter.ToUInt16(new byte[] { rxFrame.data[2], rxFrame.data[3] });
                            processData();
                        }
                        else {
                            uploadState = uploadState_t.STATE_ERROR;
                            if (uploadEvent != null) uploadEvent(eventType.ERROR_PROCESSING, 0);
                        }
                    }

                }

            }
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
            uploadState = uploadState_t.STATE_ERROR;
            Connection.refreshControl = true;
            timeoutTimer.Stop();
            if (uploadEvent != null) uploadEvent(eventType.ERROR_NO_RESPONSE, 0);           
        }        

    }
}
