using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Runtime.InteropServices;
using System.Timers;
using System.Windows.Threading;
using System.Windows.Media;

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

            public outputController(ref List<byte> config, int addressObjects, int adressChannelDescriptor, int output) {
                this.output = output;
                colorChannel = config[adressChannelDescriptor + 3];
                if (colorChannel == 0) {
                    channelColor.R = config[adressChannelDescriptor];
                    channelColor.G = config[adressChannelDescriptor + 1];
                    channelColor.B = config[adressChannelDescriptor + 2];
                }

                //loop through objects
                ledObjects = new List<ledObject>();
                bool continueLoop = true;
                bool lastBlank = false;
                double currentTime = 0;
                int currentAddress = BitConverter.ToInt32(new byte[] { config[addressObjects], config[addressObjects + 1], config[addressObjects + 2], config[addressObjects + 2] });
                while (continueLoop) {
                    byte[] objectData = new byte[12];
                    for (int i = 0; i < 12; i++) objectData[i] = config[currentAddress++];

                    switch (objectData[0]) {
                        //SOB object
                        case 0x01: {                            
                            sob led = new sob();
                            led.parseFromData(objectData, channelColor, colorChannel);
                            //if last bit is one, simply replace with empty space, else it is a real object
                            if (objectData[11] == 1) {
                                currentTime += led.length;
                                lastBlank = true;
                            }
                            else {
                                led.starttime = currentTime;
                                currentTime += led.length;
                                lastBlank = false;
                                ledObjects.Add(led);
                            }
                            continue;
                        }

                        //EOD object, last in loop
                        case 0xF0: {
                            continueLoop = false;
                            if (objectData[1] == 0) {
                                if (lastBlank) eodOption = 3;
                                else eodOption = 2;
                            }
                            if (objectData[1] == 1) eodOption = 0;
                            if (objectData[1] == 2) eodOption = 1;
                            break;
                        }

                        default: {
                            break;
                        }
                    }
                }
            }
        }

        public enum eventType {
            ERROR_NO_RESPONSE,
            ERROR_PROCESSING,
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

        public void startDownload(List<sequence_t> sequenceList) {

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

            //in config header first
            int numSequences = fullConfig[4];
            int configType = fullConfig[6];
            int configVersion = fullConfig[7];
            ushort crc = CRC.calc(fullConfig.ToArray());

            if ((crc != crcFlash) || (configType != xwCom.CONFIG_TYPE) || (configVersion != xwCom.CONFIG_VERSION)) {
                if (uploadEvent != null) uploadEvent(eventType.ERROR_PROCESSING, 0);
                return;
            }

            //here, the readed data is valid, proceed with processing
            sequenceList.Clear();

            //read offsets to seqences
            List<int> sequenceOffsetTable = new List<int> ();            
            for (int i = 0; i < numSequences; i++) {
                int offset = BitConverter.ToInt32(new byte[] { fullConfig[0x18 + i*4], fullConfig[0x18 + i * 4 + 1], fullConfig[0x18 + i * 4 + 2], fullConfig[0x18 + i * 4 + 3] });
                sequenceOffsetTable.Add (offset);
            }

            //fore each sequence
            for (int i = 0; i < numSequences; i++) {
                sequence_t sequence = new sequence_t();
                int headerAddress = sequenceOffsetTable[i];               
                
                //header information
                byte[] name = new byte[12];
                Array.Copy(fullConfig.ToArray(), headerAddress, name, 0, 12);
                sequence.name = Encoding.Default.GetString(name);

                int numoutputs = fullConfig[headerAddress + 0x0C];
                sequence.dimInfo = fullConfig[headerAddress + 0x0D];
                sequence.speedInfo = BitConverter.ToUInt32(new byte[] { fullConfig[headerAddress + 0x10], fullConfig[headerAddress + 0x11], fullConfig[headerAddress + 0x12], fullConfig[headerAddress + 0x13] });

                List<outputController> outputControllers = new List<outputController>();
                for (int j = 0; j < numoutputs; j++) {
                    int addressObjects = headerAddress + 0x1C + 4 * j;                    
                    int addressOutput = headerAddress + 0x1C + 4 * numoutputs + j;
                    int adressChannelDescriptor = headerAddress + 0x1C + 5 * numoutputs;
                    int fill = adressChannelDescriptor % 4;
                    if (fill > 0) adressChannelDescriptor += 4 - (adressChannelDescriptor % 4);
                    adressChannelDescriptor += j * 4;

                    outputControllers.Add(new outputController(ref fullConfig, addressObjects, adressChannelDescriptor, fullConfig[addressOutput]));
                }

                //combine channels
                int currentOutput = 0;
                while (currentOutput < numoutputs) {
                    outputController controller = outputControllers[currentOutput];
                    if (controller.colorChannel == 0) {
                        channel_t channel = new channel_t();
                        channel.color = controller.channelColor;
                        channel.eolOption = controller.eodOption;
                        output_t output = new output_t();
                        output.assignment = controller.output;
                        channel.outputs.Add(output);
                        foreach (ledObject ledObject in controller.ledObjects) channel.ledObjects.Add(ledObject);
                        sequence.channels.Add(channel);
                    }
                    if (controller.colorChannel == 1) {
                        outputController controllerG = outputControllers[currentOutput + 1];
                        outputController controllerB = outputControllers[currentOutput + 2];

                        channel_t channel = new channel_t();
                        channel.isRGB = true;
                        channel.eolOption = controller.eodOption;
                        foreach (ledObject ledObject in controller.ledObjects) channel.ledObjects.Add(ledObject);
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
                            setTimeout(500);                            
                            uploadState = uploadState_t.STATE_UPLOAD;
                            Connection.putFrame((byte)xwCom.SCOPE.CONFIG, new byte[] { (byte)xwCom.CONFIG.GET_FRAME, (byte) 4 });
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
                        if (bytesLoaded == 0) {
                            configSize = BitConverter.ToInt32(new byte[] { fullConfig[0], fullConfig[1], fullConfig[2], fullConfig[3] });
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
