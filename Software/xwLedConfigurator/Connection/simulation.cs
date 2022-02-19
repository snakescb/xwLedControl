using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Runtime.InteropServices;
using System.Timers;

namespace xwLedConfigurator {


    public class Simulation {

        class outputController {

            public int output;            
            int colorChannel;
            channel_t channel;
            bool isRunning;
            bool isForward;
            int lastObject;

            bool test = false;

            public struct simFrameData {
                public int numObjectsInFrame;
                public byte[] data;
            }

            public outputController(channel_t channel, int output, int colorChannel) {
                this.channel = channel;
                this.output = output;
                this.colorChannel = colorChannel;
                isRunning = true;
                isForward = true;
                lastObject = -1;
            }
            public bool canSendMore() {
                if (isRunning) return true;
                return false;
            }

            public simFrameData newFrame(int maxObjects) {

                List<byte> dataList = new List<byte>();

                int lifetime = 1000;

                for (int i = 0; i < maxObjects; i++) {

                    byte[] objectData = new byte[12];
                    objectData[0] = 0x01;
                    objectData[1] = 0xFF;
                    objectData[2] = (byte) lifetime;
                    objectData[3] = (byte) (lifetime >> 8);

                    if (!test) test = true;
                    else {
                        test = false;
                        objectData[1] = 0x00;
                    }

                    dataList.AddRange(objectData);
                }                

                simFrameData frameData = new simFrameData();
                frameData.numObjectsInFrame = maxObjects;
                frameData.data = dataList.ToArray();
                return frameData;
            }

        }

        enum simState_t {
            STOPPED,
            DISABLE_DEVICE,
            WAIT_BUFFER_INFO,
            PREFILL_BUFFER,
            RUN
        }

        simState_t simState;
        sequence_t simSequence;                
        List<outputController> outputControllers = new List<outputController>();
        byte[] currentTxData;
        int maxObjectsPerTransfer = 0;
        int maxObjectsPerOutput;     
        int preFillCurrentController;
        bool lastFrameAcknowledged;

        public Simulation() {
            simState = simState_t.STOPPED;
        }

        public void start(sequence_t sequence) {
            if (sequence.channels.Count == 0) return;
            if (simState != simState_t.STOPPED) return;

            //initialize
            simSequence = sequence;
            simState = simState_t.DISABLE_DEVICE;

            //start worker thread
            Thread simWorkerThread = new Thread(simWorker);
            simWorkerThread.Start();
        }

        public void stop() {
            simState = simState_t.STOPPED;
        }

        public void dataReceiver(ref cRxFrame rxFrame) {
            if (rxFrame.scope == (byte)xwCom.SCOPE.LED) {

                if (rxFrame.data[0] == (byte)xwCom.LED_RESPONSE.RESPONSE_ACKNOWLEDGE) {
                    if (simState == simState_t.DISABLE_DEVICE) simState = simState_t.WAIT_BUFFER_INFO;
                    if (simState == simState_t.PREFILL_BUFFER) lastFrameAcknowledged = true;
                }

                if (rxFrame.data[0] == (byte)xwCom.LED_RESPONSE.RESPONSE_BUFFER_INFO) {
                    maxObjectsPerOutput = rxFrame.data[1];
                    maxObjectsPerTransfer = rxFrame.data[2];

                    //initialize output controllers
                    outputControllers.Clear();
                    foreach (channel_t channel in simSequence.channels) {
                        if (channel.isRGB) {
                            outputControllers.Add(new outputController(channel, channel.outputs[0].assignment, 1));
                            outputControllers.Add(new outputController(channel, channel.outputs[1].assignment, 2));
                            outputControllers.Add(new outputController(channel, channel.outputs[2].assignment, 3));
                        }
                        else {
                            outputControllers.Add(new outputController(channel, channel.outputs[0].assignment, 0));
                        }
                    }

                    //intitalize prefill counters
                    preFillCurrentController = 0;
                    lastFrameAcknowledged = true;

                    //change state, start prefilling
                    simState = simState_t.PREFILL_BUFFER;
                }

            }
        }

        void sendStartSimCommand() {
            int speed = 0x00010000;
            byte dim = 0xFF;
            byte[] tx = new byte[6];
            tx[0] = (byte)xwCom.LED.START_SIM;
            tx[1] = (byte)(speed >> 24);
            tx[2] = (byte)(speed >> 16);
            tx[3] = (byte)(speed >> 8);
            tx[4] = (byte)(speed >> 0);
            tx[5] = dim;
            Connection.putFrame((byte)xwCom.SCOPE.LED, tx);
        }

        void simWorker() {
            while (simState != simState_t.STOPPED) {

                switch (simState) {

                    case simState_t.DISABLE_DEVICE:
                        //disabling device is reinitializing simulation variables in device
                        Connection.putFrame((byte)xwCom.SCOPE.LED, new byte[] { (byte)xwCom.LED.ENABLE_DISABLE, 0 });
                        Thread.Sleep(50);
                        break;


                    case simState_t.WAIT_BUFFER_INFO:
                        //request buffer information and wait a bit
                        Connection.putFrame((byte)xwCom.SCOPE.LED, new byte[] { (byte)xwCom.LED.SIM_GET_BUFFER_INFO });
                        Thread.Sleep(50);
                        break;

                    case simState_t.PREFILL_BUFFER:

                        outputController.simFrameData frame = outputControllers[0].newFrame(maxObjectsPerTransfer);

                        //prepare transmit frame
                        currentTxData = new byte[frame.data.Length + 3];
                        currentTxData[0] = (byte)xwCom.LED.SIM_SET_OBJECTS;
                        currentTxData[1] = (byte)outputControllers[0].output;
                        currentTxData[2] = (byte)frame.numObjectsInFrame;
                        frame.data.CopyTo(currentTxData, 3);

                        Connection.putFrame((byte)xwCom.SCOPE.LED, currentTxData);

                        Thread.Sleep(50);
                        simState = simState_t.RUN;
                        sendStartSimCommand();


                        /*
                        if (lastFrameAcknowledged) {
                            int maxObjects = maxNumObjects - preFillObjectsSent;
                            if (maxObjects > maxObjectsPerTransfer) maxObjects = maxObjectsPerTransfer;
                            if (maxObjects > maxObjectsPerOutput) maxObjects = maxObjectsPerOutput;

                            //if the whole buffer is filled, switch to run state
                            if (maxObjects == 0) {
                                sendStartSimCommand();
                                simState = simState_t.RUN;
                            }
                            else {
                                //ask the next controlle who wants to send something. if no more controller has to send anything, switch to run state
                                bool frameSent = false;
                                for (int i = 0; i < outputControllers.Count; i++) {
                                    if (outputControllers[preFillCurrentController].canSendMore()) {

                                        //book keeping
                                        outputController.simFrameData frame = outputControllers[preFillCurrentController].newFrame(maxObjects);
                                        preFillObjectsSent += frame.numObjectsInFrame;

                                        //prepare transmit frame
                                        currentTxData = new byte[frame.data.Length + 3];
                                        currentTxData[0] = (byte)xwCom.LED.SIM_SET_OBJECTS;
                                        currentTxData[1] = (byte) outputControllers[preFillCurrentController].output;
                                        currentTxData[2] = (byte)frame.numObjectsInFrame;
                                        frame.data.CopyTo(currentTxData, 3);

                                        //send frame
                                        Connection.putFrame((byte)xwCom.SCOPE.LED, currentTxData);
                                        lastFrameAcknowledged = false;
                                        frameSent = true;
                                        break;
                                    }
                                    else {
                                        preFillCurrentController++;
                                        if (preFillCurrentController >= outputControllers.Count) preFillCurrentController = 0;
                                    }
                                }

                                //no controller wanted to send something, so switch to run state
                                if (!frameSent) {
                                    sendStartSimCommand();
                                    simState = simState_t.RUN;
                                }
                            }
                        }
                        else {
                            //resend last frame
                            Connection.putFrame((byte)xwCom.SCOPE.LED, currentTxData);
                        }
                        */

                        Thread.Sleep(20);
                        break;

                    case simState_t.RUN:

                        Thread.Sleep(20);
                        break;


                    default:
                        break;
                }

            }
        }


    }
}
