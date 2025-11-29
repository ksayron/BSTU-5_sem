const WebSocket = require('ws');

const socket = new WebSocket('ws://localhost:3000');

socket.on('open', () => {
    console.log('поключился к  серверу');
});

socket.on('message', (data) => {
    
    try {
        const msg = JSON.parse(data);
            console.log('[MESSAGE]', msg);
    } catch (e) {
        console.log('[MESSAGE]', data.toString());
    }
});
socket.on('close', () => {
    console.log('отлючился от сервера');
});