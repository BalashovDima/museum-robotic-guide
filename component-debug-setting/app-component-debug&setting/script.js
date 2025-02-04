let port;
let writer;

function sendStepperCommand() {
    const degrees = document.getElementById('stepperDegrees').value;
    if (degrees === '' || isNaN(degrees)) {
        alert('Please enter a valid number for degrees.');
        return;
    }
    sendCommand(`M${degrees}`);
}

function sendServoCommand() {
    const degrees = document.getElementById('servoDegrees').value;
    if (degrees === '' || isNaN(degrees) || degrees < 0 || degrees > 180) {
        alert('Please enter a valid number between 0 and 180.');
        return;
    }
    sendCommand(`S${degrees}`);
}

// Object to store the state of each GPIO pin (initialized as ON by default)
const gpioStates = {};

// Function to generate GPIO buttons
function createGpioButtons() {
    const gpioContainer0to7 = document.getElementById('gpio-buttons-0-7');
    const gpioContainer8to15 = document.getElementById('gpio-buttons-8-15');

    // Create buttons for pins 0-7 (Module 1)
    for (let pin = 0; pin < 8; pin++) {
        const button = document.createElement('button');
        button.textContent = `${pin} (ON)`; // Default state: ON
        button.dataset.pin = pin;
        button.style.backgroundColor = 'green'; // Default color: green
        button.onclick = () => toggleGpioPin(pin);
        gpioContainer0to7.appendChild(button);
        gpioStates[pin] = true; // Initialize state to ON (LOW)
    }

    // Create buttons for pins 8-15 (Module 2)
    for (let pin = 8; pin < 16; pin++) {
        const button = document.createElement('button');
        button.textContent = `${pin} (ON)`; // Default state: ON
        button.dataset.pin = pin;
        button.style.backgroundColor = 'green'; // Default color: green
        button.onclick = () => toggleGpioPin(pin);
        gpioContainer8to15.appendChild(button);
        gpioStates[pin] = true; // Initialize state to ON (LOW)
    }
}

// Function to toggle GPIO pin state
function toggleGpioPin(pin) {
    const module = pin < 8 ? 1 : 2; // Determine module based on pin number
    const pinNumber = pin < 8 ? pin : pin - 8; // Map pin to 0-7 for the module
    const newState = !gpioStates[pin]; // Toggle state

    // Send command to Arduino
    const command = newState ? 'H' : 'L'; // L for LOW (ON), H for HIGH (OFF)
    sendCommand(`${command}${pin}`);

    // Update button appearance
    const button = document.querySelector(`button[data-pin="${pin}"]`);
    button.style.backgroundColor = newState ? 'green' : 'red';
    button.textContent = `${pin} (${newState ? 'ON' : 'OFF'})`;

    // Update state in the object
    gpioStates[pin] = newState;
}

// Call the function to create buttons when the page loads
document.addEventListener('DOMContentLoaded', createGpioButtons);

// Connect button (optional)
document.addEventListener('DOMContentLoaded', () => {
    const connectButton = document.createElement('button');
    connectButton.textContent = 'Connect to Arduino';
    connectButton.onclick = connect;
    document.querySelector('.container').prepend(connectButton);
});

// Function to log messages to the serial console
function logToConsole(message) {
    const consoleElement = document.getElementById('serial-console');
    consoleElement.value += `${message}\n`; // Append the message
    consoleElement.scrollTop = consoleElement.scrollHeight; // Auto-scroll to the bottom
}

// Function to clear the console
function clearConsole() {
    const consoleElement = document.getElementById('serial-console');
    consoleElement.value = ''; // Clear the console
}

// Function to send a command to the Arduino and log it
async function sendCommand(command) {
    if (!writer) {
        alert('Please connect to the Arduino first.');
        return;
    }
    const encoder = new TextEncoder();
    await writer.write(encoder.encode(command + '\n'));
    logToConsole(`<- ${command}`); // Log the sent command
}

let message = '';
// Function to read and log received messages from the Arduino
async function readSerial() {
    const reader = port.readable.getReader();
    const decoder = new TextDecoder();
    try {
        while (true) {
            const { value, done } = await reader.read();
            if (done) break;
            const text = decoder.decode(value);
            if(text === '\n') {
                logToConsole(`-> ${message}`); // Log the received message
                message = '';
            } else {
                message += text;
            }
        }
    } catch (error) {
        console.error('Error reading serial data:', error);
    } finally {
        reader.releaseLock();
    }
}

// Update the connect function to start reading serial data
async function connect() {
    try {
        port = await navigator.serial.requestPort();
        await port.open({ baudRate: 9600 });
        writer = port.writable.getWriter();
        document.getElementById('status').textContent = 'Connected';
        logToConsole('Connected to Arduino'); // Log connection status

        // Start reading serial data
        readSerial();
    } catch (error) {
        document.getElementById('status').textContent = 'Connection failed';
        logToConsole('Connection failed'); // Log connection failure
        console.error(error);
    }
}