const ws = require('ws');

function startClient(id = '') {
  const ws = new ws('ws://localhost:4000');

  let seq = 0;
  let interval = null;
  let stopTimeout = null;

  ws.on('open', () => {
    console.log(`[${id}] ws connection open`);
    interval = setInterval(() => {
      seq++;
      const msg = `10-01-client: ${seq}`;
      ws.send(msg);
      console.log(`[${id}] sent:`, msg);
    }, 3000);

    stopTimeout = setTimeout(() => {
      clearInterval(interval);
      console.log(`[${id}] stopping after 25s, closing ws`);
      ws.close();
    }, 25000);
  });

  ws.on('message', data => {
    console.log(`[${id}] recv:`, data.toString());
  });

  ws.on('close', () => {
    console.log(`[${id}] closed`);
    clearInterval(interval);
    clearTimeout(stopTimeout);
  });

  ws.on('error', err => {
    console.error(`[${id}] error`, err);
  });
}

startClient('client1');
startClient('client2');