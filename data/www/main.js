document.addEventListener('DOMContentLoaded', function() {
    // Replace 'yourWebSocketServerURL' with the actual WebSocket server URL
    const socket = new WebSocket('ws://'+window.location.hostname+'/ws');
    //const socket = new WebSocket('ws://'+'192.168.2.45'+'/ws');

    // Connection opened
    socket.addEventListener('open', (event) => {
        console.log('Connected to the WebSocket server');
        
        // Sending data to the server after connection is open
        //socket.send('Hello, server!');
    });

    // Listen for messages from the server
    socket.addEventListener('message', (event) => {
        //console.log('Message from server:', event.data);
        var data = JSON.parse(event.data);
        console.log(data);
        if(data.rssi){
            var element = document.getElementById('rssi');
            element.innerHTML = data.rssi;
        }
        if(data.temperature){
            var element = document.getElementById('temperature');
            element.innerHTML = data.temperature;
        }
        if(data.humidity){
            var element = document.getElementById('humidity');
            element.innerHTML = data.humidity;
        }
        if(data.ip){
            var element = document.getElementById('ip');
            element.innerHTML = data.ip;
        }
        if(data.ssid){
            var element = document.getElementById('ssid');
            element.innerHTML = data.ssid;
        }
        if(data.uptime){
            var element = document.getElementById('uptime');
            element.innerHTML = data.uptime;
        }
        
    });

    // Listen for any errors that occur.
    socket.addEventListener('error', (event) => {
        console.error('WebSocket Error:', event);
    });

    // Connection closed
    socket.addEventListener('close', (event) => {
        if (event.wasClean) {
            console.log(`Connection closed cleanly, code=${event.code}, reason=${event.reason}`);
        } else {
            console.error('Connection abruptly closed');
        }
    });
 });