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
            public byte dim;
            int colorChannel;
            channel_t channel;
            bool stopped;
            bool forward;
            bool insertBlankEnd;
            double currentTime;
            int currentObject;
            byte[] breakFrame;

            public struct simFrameData {
                public int numObjectsInFrame;
                public byte[] data;
            }

            public outputController(channel_t channel, int output, int colorChannel) {
                this.channel = channel;
                this.output = output;
                this.colorChannel = colorChannel;
                this.dim = channel.channelDim;
                stopped = false;
                forward = true;
                insertBlankEnd = false;
                currentTime = 0;
                currentObject = 0;
                breakFrame = new byte[12];
                breakFrame[0] = 0x01;
                
                if (channel.ledObjects.Count == 0) stopped = true;                            
            }

            public bool canSendMore() {
                if (stopped) return false;
                return true;
            }

            public simFrameData newFrame(int maxObjects) {

                simFrameData frameData = new simFrameData();
                frameData.numObjectsInFrame = 0;
                List<byte> dataList = new List<byte>();

                if (stopped) return frameData;
                if (maxObjects == 0) return frameData;
                
                bool continueLoop = true;
                int objectsPrepared = 0;
                while (continueLoop && !stopped) {

                    //insert a 1ms long 0 brightness sob, then stop output
                    if (insertBlankEnd) {
                        breakFrame[2] = 0x01;
                        breakFrame[3] = 0x00;
                        dataList.AddRange(breakFrame);
                        objectsPrepared++;
                        stopped = true;
                        continue;
                    }
                    
                    ledObject led = channel.ledObjects[currentObject];

                    if (forward) {
                        //check if at the current time there is an object    
                        if (currentTime == led.starttime) {
                            //add data of this object to list
                            dataList.AddRange(led.getBuffer(colorChannel, false));
                            objectsPrepared++;
                            if (objectsPrepared == maxObjects) continueLoop = false;
                            currentTime += led.length;
                            currentObject++;

                            //when the last object in the channel was processed
                            if (currentObject == channel.ledObjects.Count) {
                                //check end of line option for this channel
                                //Restart from beginngin
                                if (channel.eolOption == 0) {
                                    currentObject = 0;
                                    currentTime = 0;
                                }
                                //start reversed
                                if (channel.eolOption == 1) {
                                    forward = false;
                                    currentObject = channel.ledObjects.Count - 1;
                                    currentTime = channel.ledObjects[currentObject].starttime + channel.ledObjects[currentObject].length - 1;
                                }
                                //stop output with last brigthness
                                if (channel.eolOption == 2) stopped = true;
                                //stop with output disabled, requires to insert a blank end frame
                                if (channel.eolOption == 3) insertBlankEnd = true;
                            }
                            continue;
                        }
                        else {
                            //we need to insert a blank pause element (sob with brightness 0)
                            long breakLen = (long)(led.starttime - currentTime);
                            breakFrame[2] = (byte)(breakLen);
                            breakFrame[3] = (byte)(breakLen >> 8);
                            dataList.AddRange(breakFrame);
                            currentTime = led.starttime;
                            objectsPrepared++;
                            if (objectsPrepared == maxObjects) continueLoop = false;
                            continue;
                        }
                    }
                    else {
                        //reverse mode
                        if (currentTime == led.starttime + led.length - 1) {
                            //add reversed data of this object to list
                            dataList.AddRange(led.getBuffer(colorChannel, true));
                            objectsPrepared++;
                            if (objectsPrepared == maxObjects) continueLoop = false;
                            currentTime -= led.length;
                            if (currentObject > 0) currentObject--;

                            //when the time pointer is below 0, restart channel in forward mode
                            if (currentTime < 0) {
                                currentObject = 0;
                                currentTime = 0;
                                forward = true;
                            }
                            continue;
                        }
                        else {
                            //we need to insert a blank pause element (sob with brightness 0)
                            long breakLen;
                            if ((currentObject == 0) && (currentTime < led.starttime)) {
                                //this will be the last break before the channel is reversed again
                                breakLen = (long)led.starttime;
                                currentTime = 0;
                                forward = true;
                            }
                            else {
                                breakLen = (long)(currentTime - led.starttime - led.length + 1);
                                currentTime = led.starttime + led.length - 1;
                            }
                            breakFrame[2] = (byte)(breakLen);
                            breakFrame[3] = (byte)(breakLen >> 8);
                            dataList.AddRange(breakFrame);
                            objectsPrepared++;
                            if (objectsPrepared == maxObjects) continueLoop = false;
                        }
                    }
                }

                //return prepared data
                frameData.numObjectsInFrame = objectsPrepared;
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
        int maxOutputs;
        int objectCounter;
        int currentController;
        bool lastFrameAcknowledged;
        int[] availableBuffers;

        public Simulation() {
            simState = simState_t.STOPPED;
        }

        public void start(sequence_t sequence) {
            if (sequence.channels.Count == 0) return;

            if (simState != simState_t.STOPPED) {
                simState = simState_t.STOPPED;
                Thread.Sleep(120);
            }

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
                    maxOutputs = rxFrame.data[3];
                    availableBuffers = new int[maxOutputs];

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
                    currentController = 0;
                    objectCounter = 0;
                    lastFrameAcknowledged = true;

                    //change state, start prefilling
                    simState = simState_t.PREFILL_BUFFER;
                }

                if(rxFrame.data[0] == (byte)xwCom.LED_RESPONSE.RESPONSE_BUFFER_STATE) {

                    //for each channel, check how many new objects can be sent. Send new frame when 8 or more objects can be sent to reduce amount of frames
                    foreach (outputController controller in outputControllers) {
                        if (controller.canSendMore()) {
                            int availbleBuffers = maxObjectsPerOutput - rxFrame.data[controller.output + 1];
                            if (availbleBuffers >= maxObjectsPerTransfer) {

                                //get the objects for this output
                                outputController.simFrameData frame = controller.newFrame(maxObjectsPerTransfer);

                                //send new object to device
                                if (frame.numObjectsInFrame > 0) {
                                    //prepare and send frame
                                    byte[] txdata = new byte[frame.data.Length + 4];
                                    txdata[0] = (byte)xwCom.LED.SIM_SET_OBJECTS;
                                    txdata[1] = (byte)controller.output;
                                    txdata[2] = (byte)controller.dim;
                                    txdata[3] = (byte)frame.numObjectsInFrame;
                                    frame.data.CopyTo(txdata, 4);

                                    Connection.putFrame((byte)xwCom.SCOPE.LED, txdata);
                                }
                            }
                        }
                    }
                }
            }
        }

        void sendStartSimCommand() {
            long speed = simSequence.speedInfo;
            byte dim = (byte) simSequence.dimInfo;
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

                        if (lastFrameAcknowledged) {
                            int maxObjects = maxObjectsPerOutput - objectCounter;
                            if (maxObjects > maxObjectsPerTransfer) maxObjects = maxObjectsPerTransfer;

                            //if the buffer for an output is full, or the controler does not have more objects, go to the next controller
                            if ((maxObjects == 0) || !outputControllers[currentController].canSendMore()) {
                                objectCounter = 0;
                                currentController++;
                                if (currentController == outputControllers.Count) {
                                    //all controllers done, start simulation
                                    sendStartSimCommand();
                                    simState = simState_t.RUN;
                                    Thread.Sleep(50);
                                    break;
                                }
                            }
                            else {
                                //get the objects for the current controller
                                outputController.simFrameData frame = outputControllers[currentController].newFrame(maxObjects);

                                //check how many objects wil be sent
                                objectCounter += frame.numObjectsInFrame;

                                //prepare transmit frame
                                currentTxData = new byte[frame.data.Length + 4];
                                currentTxData[0] = (byte)xwCom.LED.SIM_SET_OBJECTS;
                                currentTxData[1] = (byte)outputControllers[currentController].output;
                                currentTxData[2] = (byte)outputControllers[currentController].dim;
                                currentTxData[3] = (byte)frame.numObjectsInFrame;
                                frame.data.CopyTo(currentTxData, 4);

                                //send frame
                                Connection.putFrame((byte)xwCom.SCOPE.LED, currentTxData);
                                lastFrameAcknowledged = false;
                            }
                        }
                        else {
                            //resend last frame
                            Connection.putFrame((byte)xwCom.SCOPE.LED, currentTxData);
                        }    

                        Thread.Sleep(50);
                        break;

                    case simState_t.RUN:

                        //request buffer update
                        Connection.putFrame((byte)xwCom.SCOPE.LED, new byte[] { (byte)xwCom.LED.SIM_GET_BUFFER_STATE });
                        Thread.Sleep(50);
                        break;


                    default:
                        break;
                }

            }
        }


    }
}
