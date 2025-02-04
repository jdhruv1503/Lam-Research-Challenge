import { interTight } from './fonts'
import './globals.css'

export default function RootLayout({
  children,
}: {
  children: React.ReactNode
}) {
  return (
    <html lang="en" className={`dark ${interTight.variable}`}>
      <head>
        <title>Robotic Arm Control Thingy
        </title>
      </head>
      <body>{children}</body>
    </html>
  )
}

