const net = require('net')

const SOCKET_PATH = "/tmp/node-to-c-unix-socket.sock"

main()
  .catch((e) => console.error('Unhandled rejection:', e))

async function main () {
  try {
    let buffer = Buffer.from('hithere', 'utf8')
    let data = await sendAndReceiveData(buffer)
    console.log('Got data:', data.toString('utf8'))
  } catch (e) {
    console.error('Got error:', e)
  }
}

async function sendAndReceiveData (data) {
  // Assumes that data is a Buffer.
  if (data.length > 255) {
    throw new Error('Too much data to send!')
  }

  let bufLen = new Buffer(1)
  bufLen.writeUInt8(data.length, 0)

  let promise = new Promise((resolve, reject) => {
    let client = net.connect(SOCKET_PATH, () => {
      console.log('Connected to socket')
    })
    
    client.on('error', (e) => {
      reject(new Error('Got error from server' + e))
    })

    // First write data.
    client.write(bufLen)
    client.write(data)

    // Then wait for response.

    let respBuf = Buffer.alloc(256)
    let bufPos = 0
    let respLen = 0

    client.on('data', (data) => {
      // If this is the first byte, it's the length.
      if (respLen === 0) {
        respLen = data.readUInt8(0)
        data = data.slice(1)
      }
      bufPos += data.copy(respBuf, bufPos)
    })

    client.on('end', () => {
      if (bufPos === respLen) {
        resolve(respBuf)
      } else {
        reject(new Error(`EOF before receiving all data! respBuf.length: ${bufPos}, respLen: ${respLen}`))
      }
    })
  })
  return promise
}


