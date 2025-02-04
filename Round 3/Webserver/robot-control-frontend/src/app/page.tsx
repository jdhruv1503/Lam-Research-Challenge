'use client'

import React, { useState, useCallback } from 'react'
import { Send, Move, Layers, Download, Loader2, Code } from 'lucide-react'
import { Card, CardContent, CardFooter, CardHeader, CardTitle } from "@/components/ui/card"
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs"
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Slider } from "@/components/ui/slider"
import { Alert, AlertDescription, AlertTitle } from "@/components/ui/alert"

const RobotController = () => {
  const [x, setX] = useState(0)
  const [y, setY] = useState(0)
  const [z, setZ] = useState(0)
  const [speed, setSpeed] = useState(50)
  const [acceleration, setAcceleration] = useState(50)
  const [diskId, setDiskId] = useState(0)
  const [lastResponse, setLastResponse] = useState('')
  const [commandStatus, setCommandStatus] = useState('')
  const [isLoading, setIsLoading] = useState(false)
  const [serialCommand, setSerialCommand] = useState('')

  const getStatus = useCallback(async () => {
    setIsLoading(true)
    setLastResponse('')
    setCommandStatus('')

    try {
      const response = await fetch('http://localhost:5000/get_status')
      const data = await response.json()
      setLastResponse(data.responses ? data.responses.join(', ') : (data.error || 'No response'))
      setCommandStatus(data.status || 'unknown')
      setIsLoading(data.status !== 'success' && !data.error)
    } catch (error) {
      setLastResponse(`Error: ${error.message}`)
      setCommandStatus('error')
      setIsLoading(false)
    }
  }, [])

  const sendCommand = useCallback(async (command) => {
    setIsLoading(true)
    setLastResponse('')
    setCommandStatus('')

    try {
      const response = await fetch('http://localhost:5000/command', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ command }),
      })
      const data = await response.json()
      
      setLastResponse(data.responses ? data.responses.join(', ') : (data.error || 'No response'))
      setCommandStatus(data.status || 'unknown')
      setIsLoading(data.status !== 'success' && !data.error)
      getStatus()
    } catch (error) {
      setLastResponse(`Error: ${error.message}`)
      setCommandStatus('error')
      setIsLoading(false)
    }
  }, [getStatus])

  const sendSerialCommand = useCallback(async (command) => {
    setIsLoading(true)
    setLastResponse('')
    setCommandStatus('')

    try {
      const response = await fetch('http://localhost:5000/serial_command', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ command }),
      })
      const data = await response.json()
      
      setLastResponse(data.responses ? data.responses.join(', ') : (data.error || 'No response'))
      setCommandStatus(data.status || 'unknown')
      setIsLoading(data.status !== 'success' && !data.error)
      getStatus()
    } catch (error) {
      setLastResponse(`Error: ${error.message}`)
      setCommandStatus('error')
      setIsLoading(false)
    }
  }, [getStatus])

  const renderArmControlTab = () => (
    <div className="grid grid-cols-1 gap-8 pt-16">
      {[
        { name: 'Base', code: '01' },
        { name: 'ARM1', code: '02' },
        { name: 'ARM2', code: '03' },
        { name: 'End Effector', code: '04' }
      ].map((arm) => (
        <div key={arm.code} className="space-y-4">
          <h3 className="text-lg font-medium text-gray-200">{arm.name} Control</h3>
          <div className="grid grid-cols-2 gap-4">
            <div className="space-y-2">
              <div className="grid grid-cols-2 gap-2">
                <Button
                  onClick={() => sendSerialCommand(`$${arm.code}*02*5#`)}
                  disabled={isLoading}
                  className="w-full bg-blue-600 hover:bg-blue-700"
                >
                  {isLoading ? <Loader2 className="animate-spin mx-auto" /> : "-5°"}
                </Button>
                <Button
                  onClick={() => sendSerialCommand(`$${arm.code}*02*10#`)}
                  disabled={isLoading}
                  className="w-full bg-blue-600 hover:bg-blue-700"
                >
                  {isLoading ? <Loader2 className="animate-spin mx-auto" /> : "-10°"}
                </Button>
              </div>
            </div>
            <div className="space-y-2">
              <div className="grid grid-cols-2 gap-2">
                <Button
                  onClick={() => sendSerialCommand(`$${arm.code}*01*5#`)}
                  disabled={isLoading}
                  className="w-full bg-green-600 hover:bg-green-700"
                >
                  {isLoading ? <Loader2 className="animate-spin mx-auto" /> : "+5°"}
                </Button>
                <Button
                  onClick={() => sendSerialCommand(`$${arm.code}*01*10#`)}
                  disabled={isLoading}
                  className="w-full bg-green-600 hover:bg-green-700"
                >
                  {isLoading ? <Loader2 className="animate-spin mx-auto" /> : "+10°"}
                </Button>
              </div>
            </div>
          </div>
        </div>
      ))}
      <div className="space-y-4">
        <h3 className="text-lg font-medium text-gray-200">End Effector Actions</h3>
        <div className="grid grid-cols-2 gap-4">
          <Button
            onClick={() => sendSerialCommand('$05*04*01#')}
            disabled={isLoading}
            className="w-full bg-purple-600 hover:bg-purple-700"
          >
            {isLoading ? <Loader2 className="animate-spin mx-auto" /> : "Pick"}
          </Button>
          <Button
            onClick={() => sendSerialCommand('$05*05*00#')}
            disabled={isLoading}
            className="w-full bg-orange-600 hover:bg-orange-700"
          >
            {isLoading ? <Loader2 className="animate-spin mx-auto" /> : "Drop"}
          </Button>
        </div>
      </div>
    </div>
  )

  const renderSerialTab = () => (
    <div className="space-y-4 pt-16">
      <div className="space-y-2">
        <label className="text-sm font-medium text-gray-200">Serial Command</label>
        <Input
          type="text"
          placeholder="Enter command (e.g. $01*01*120#)"
          value={serialCommand}
          onChange={(e) => setSerialCommand(e.target.value)}
          disabled={isLoading}
          className="bg-gray-700 text-white border-gray-600"
        />
        <Button
          onClick={() => {
            // Validate command format
            const cmdRegex = /^\$(\d{2})\*(\d{2})\*(\d+)#$/;
            const match = serialCommand.match(cmdRegex);
            
            if (!match) {
              setLastResponse("Invalid command format");
              setCommandStatus("error");
              return;
            }

            const [_, funcn, action, value] = match;
            
            // Validate function code
            if (!["01","02","03","04","05"].includes(funcn)) {
              setLastResponse("Invalid function code");
              setCommandStatus("error"); 
              return;
            }

            // Validate action code
            if (!["01","02","03","04","05"].includes(action)) {
              setLastResponse("Invalid action code");
              setCommandStatus("error");
              return;
            }

            // Validate value based on action
            if (["01","02","03"].includes(action)) {
              const angle = parseInt(value);
              if (angle < 0 || angle > 360) {
                setLastResponse("Angle must be between 0-360");
                setCommandStatus("error");
                return;
              }
            } else if (["04","05"].includes(action)) {
              if (!["00","01"].includes(value)) {
                setLastResponse("Invalid end effector value");
                setCommandStatus("error");
                return;
              }
            }

            sendSerialCommand(serialCommand);
          }}
          disabled={isLoading}
          className="w-full bg-blue-600 hover:bg-blue-700"
        >
          {isLoading ? <Loader2 className="animate-spin mx-auto" /> : "Send Command"}
        </Button>
      </div>
    </div>
  )

  const renderSpeedTab = () => (
    <div className="space-y-4">
      <div className="grid grid-cols-2 gap-4 pt-16">
        {['Speed', 'Acceleration'].map((param) => (
          <div key={param} className="space-y-2">
            <label className="text-sm font-medium text-gray-200">{param} (%)</label>
            <Slider
              min={0}
              max={100}
              step={1}
              value={[param === 'Speed' ? speed : acceleration]}
              onValueChange={(value) => param === 'Speed' ? setSpeed(value[0]) : setAcceleration(value[0])}
              disabled={isLoading}
              className="bg-gray-700"
            />
            <div className="text-center text-gray-200">{param === 'Speed' ? speed : acceleration}%</div>
          </div>
        ))}
      </div>
      <div className="grid grid-cols-3 gap-4 pt-16">
        {['X', 'Y', 'Z'].map((axis) => (
          <React.Fragment key={axis}>
            <Button 
              onClick={() => sendCommand(`set speed ${axis.toLowerCase()} ${speed}`)}
              disabled={isLoading}
              className={`w-full ${axis === 'X' ? 'bg-blue-600 hover:bg-blue-700' : axis === 'Y' ? 'bg-green-600 hover:bg-green-700' : 'bg-red-600 hover:bg-red-700'}`}
            >
              {isLoading ? <Loader2 className="animate-spin mx-auto" /> : `Set ${axis} Speed`}
            </Button>
            <Button 
              onClick={() => sendCommand(`set accel ${axis.toLowerCase()} ${acceleration}`)}
              disabled={isLoading}
              className={`w-full ${axis === 'X' ? 'bg-blue-600 hover:bg-blue-700' : axis === 'Y' ? 'bg-green-600 hover:bg-green-700' : 'bg-red-600 hover:bg-red-700'}`}
            >
              {isLoading ? <Loader2 className="animate-spin mx-auto" /> : `Set ${axis} Accel`}
            </Button>
          </React.Fragment>
        ))}
      </div>
    </div>
  )

  const renderMacroTab = () => (
    <div className="space-y-4">
      <div className="space-y-2">
        <label className="text-sm font-medium text-gray-200">Disk ID</label>
        <Input 
          type="number" 
          value={diskId} 
          onChange={(e) => setDiskId(parseInt(e.target.value))}
          disabled={isLoading}
          className="bg-gray-700 text-white border-gray-600"
        />
      </div>
      <div className="grid grid-cols-2 gap-4">
        <Button 
          onClick={() => sendCommand(`pickup ${diskId}`)}
          disabled={isLoading}
          className="bg-purple-600 hover:bg-purple-700"
        >
          {isLoading ? <Loader2 className="animate-spin mx-auto" /> : 'Pick Up Disk'}
        </Button>
        <Button 
          onClick={() => sendCommand(`dropoff ${diskId}`)}
          disabled={isLoading}
          className="bg-orange-600 hover:bg-orange-700"
        >
          {isLoading ? <Loader2 className="animate-spin mx-auto" /> : 'Drop Off Disk'}
        </Button>
      </div>
    </div>
  )

  return (
    <div className="min-h-screen text-white font-inter-tight">
      <div className="absolute top-0 z-[-2] h-screen w-screen bg-neutral-950 bg-[radial-gradient(ellipse_80%_80%_at_50%_-20%,rgba(120,119,198,0.3),rgba(255,255,255,0))]"></div>
      <div className="container mx-auto px-4 py-8">
        <h1 className="text-4xl font-bold text-center mb-2 pt-[15vh] ">Robotic Arm Control Thingy</h1>
        <h2 className="text-xl text-gray-400 text-center mb-8 pb-[10vh]">Made with ❤️ by team π-Propulsion for the <a href="https://www.tworks.in/lam-challenge" className="underline hover:text-gray-300">Lam Research Challenge 2024</a>!</h2>
        <Card className="w-full max-w-4xl mx-auto bg-gray-800 border-gray-700 p-16">
          <CardContent>
            <Tabs defaultValue="arm-control">
              <TabsList className="grid w-full grid-cols-4 bg-gray-700">
                <TabsTrigger value="arm-control" disabled={isLoading} className="data-[state=active]:bg-gray-600">
                  <Move className="mr-2" /> Arm Control
                </TabsTrigger>
                <TabsTrigger value="serial" disabled={isLoading} className="data-[state=active]:bg-gray-600">
                  <Code className="mr-2" /> Serial Control
                </TabsTrigger>
                <TabsTrigger value="speed" disabled={isLoading} className="data-[state=active]:bg-gray-600">
                  <Layers className="mr-2" /> Speed & Acceleration
                </TabsTrigger>
                <TabsTrigger value="macro" disabled={isLoading} className="data-[state=active]:bg-gray-600">
                  <Send className="mr-2" /> Macro Commands
                </TabsTrigger>
              </TabsList>

              <TabsContent value="arm-control" className="mt-4">
                {renderArmControlTab()}
              </TabsContent>
              <TabsContent value="serial" className="mt-4">
                {renderSerialTab()}
              </TabsContent>
              <TabsContent value="speed" className="mt-4">
                {renderSpeedTab()}
              </TabsContent>
              <TabsContent value="macro" className="mt-4">
                {renderMacroTab()}
              </TabsContent>
            </Tabs>
          </CardContent>
          <CardFooter className="flex-col space-y-4">
            <Button 
              onClick={getStatus}
              disabled={isLoading}
              className="w-full bg-gray-600 hover:bg-gray-700"
            >
              {isLoading ? <Loader2 className="animate-spin mr-2" /> : <Download className="mr-2" />}
              Get Status
            </Button>
            {(lastResponse || commandStatus) && (
              <Alert variant={commandStatus === 'success' ? 'default' : commandStatus === 'error' ? 'destructive' : 'default'} className="bg-gray-700 border-gray-600">
                <AlertTitle className="text-gray-100">Status: {commandStatus}</AlertTitle>
                <AlertDescription className="text-gray-300">{lastResponse}</AlertDescription>
              </Alert>
            )}
          </CardFooter>
        </Card>
      </div>
    </div>
  )
}

export default RobotController
