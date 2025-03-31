from flask import Flask, render_template_string, request
import subprocess
import threading
import time

app = Flask(__name__)
motor_proc = subprocess.Popen(['./motor'], stdin=subprocess.PIPE)

# 线程控制变量
current_thread = None
stop_event = threading.Event()

key_map = {
    'up': b'\x1b[A',
    'down': b'\x1b[B',
    'left': b'\x1b[D',
    'right': b'\x1b[C'
}

def send_direction(direction):
    while not stop_event.is_set():
        motor_proc.stdin.write(key_map[direction])
        motor_proc.stdin.flush()
        time.sleep(0.05)

@app.route('/control', methods=['POST'])
def control():
    global current_thread, stop_event
    direction = request.form.get('direction')
    action = request.form.get('action')

    if action == 'press':
        if current_thread is not None:
            stop_event.set()
            current_thread.join()
        
        stop_event.clear()
        current_thread = threading.Thread(target=send_direction, args=(direction,))
        current_thread.start()
    elif action == 'release':
        stop_event.set()
        current_thread = None
    
    return 'OK'

@app.route('/click', methods=['POST'])
def handle_click():
    direction = request.form.get('direction')
    motor_proc.stdin.write(key_map[direction])
    motor_proc.stdin.flush()
    return 'OK'

@app.route('/')
def index():
    return render_template_string('''
        <html>
        <head>
            <title>小车控制</title>
            <style>
                .container {
                    display: grid;
                    grid-template-columns: repeat(3, 100px);
                    grid-template-rows: repeat(3, 100px);
                    gap: 10px;
                    justify-content: center;
                    margin-top: 50px;
                }
                .btn {
                    width: 100px;
                    height: 100px;
                    font-size: 24px;
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    background-color: #f0f0f0;
                    border: 2px solid #333;
                    border-radius: 10px;
                    cursor: pointer;
                    user-select: none;
                    -webkit-user-select: none;
                    -webkit-touch-callout: none;
                    -webkit-tap-highlight-color: transparent;
                }
                .btn:active { background-color: #ddd; }
                #up { grid-column: 2; }
                #down { grid-column: 2; grid-row: 3; }
                #left { grid-column: 1; grid-row: 2; }
                #right { grid-column: 3; grid-row: 2; }
            </style>
        </head>
        <body>
            <div class="container">
                <div class="btn" id="up" 
                    ontouchstart="handleStart(event, 'up')"
                    ontouchend="handleEnd(event, 'up')"
                    onmousedown="handleStart(event, 'up')"
                    onmouseup="handleEnd(event, 'up')">↑</div>
                
                <div class="btn" id="left"
                    ontouchstart="handleStart(event, 'left')"
                    ontouchend="handleEnd(event, 'left')"
                    onmousedown="handleStart(event, 'left')"
                    onmouseup="handleEnd(event, 'left')">←</div>
                
                <div class="btn" id="right"
                    ontouchstart="handleStart(event, 'right')"
                    ontouchend="handleEnd(event, 'right')"
                    onmousedown="handleStart(event, 'right')"
                    onmouseup="handleEnd(event, 'right')">→</div>
                
                <div class="btn" id="down"
                    ontouchstart="handleStart(event, 'down')"
                    ontouchend="handleEnd(event, 'down')"
                    onmousedown="handleStart(event, 'down')"
                    onmouseup="handleEnd(event, 'down')">↓</div>
            </div>

            <script>
                let pressTimer;
                let isLongPress = false;
                
                function handleStart(event, direction) {
                    event.preventDefault();
                    isLongPress = false;
                    pressTimer = setTimeout(() => {
                        isLongPress = true;
                        sendCommand(direction, 'press');
                    }, 500); // 长按时间阈值500ms
                }

                function handleEnd(event, direction) {
                    event.preventDefault();
                    clearTimeout(pressTimer);
                    
                    if (isLongPress) {
                        sendCommand(direction, 'release');
                        return;
                    }
                    
                    // 单击处理
                    fetch('/click', {
                        method: 'POST',
                        headers: {
                            'Content-Type': 'application/x-www-form-urlencoded',
                        },
                        body: `direction=${direction}`
                    });
                }

                function sendCommand(direction, action) {
                    fetch('/control', {
                        method: 'POST',
                        headers: {
                            'Content-Type': 'application/x-www-form-urlencoded',
                        },
                        body: `direction=${direction}&action=${action}`
                    });
                }
            </script>
        </body>
        </html>
    ''')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, threaded=True)