#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char index_html[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Touch Controller</title>
  <style>
    body {
      font-family: sans-serif;
      text-align: center;
      background: #f0f0f0;
      margin: 0;
      padding: 0;
    }
    h1 {
      margin-top: 20px;
    }
    .controller {
      display: grid;
      grid-template-columns: 1fr 1fr 1fr;
      grid-template-rows: 1fr 1fr 1fr;
      gap: 10px;
      width: 300px;
      margin: 40px auto;
      touch-action: none;
    }
    .btn {
      background-color: #3498db;
      color: white;
      font-size: 1.2rem;
      border: none;
      border-radius: 12px;
      padding: 20px;
      user-select: none;
      transition: background-color 0.2s;
    }
    .btn:active,
    .btn.active {
      background-color: #2980b9;
    }
    #auto {
      width: 200px;
      margin: 10px auto;
      background-color: #e67e22;
    }
    #auto:active,
    #auto.active {
      background-color: #d35400;
    }
    button {
      user-select: none;
      -webkit-user-select: none;  /* Safari */
      -ms-user-select: none;      /* IE 10+ */
      touch-action: manipulation; /* Improves touch response */
    }
  </style>
  <script>
    let socket;

    function sendWSCommand(cmd, state) {
      if (socket && socket.readyState === WebSocket.OPEN) {
        socket.send(JSON.stringify({ cmd: cmd, state: state }));
      }
    }

    function registerButton(id, cmd, isToggle = false) {
      const btn = document.getElementById(id);
      btn.addEventListener("pointerdown", e => {
        e.preventDefault();
        if (isToggle) {
          btn.classList.toggle("active");
          sendWSCommand(cmd, "toggle");
        } else {
          btn.classList.add("active");
          sendWSCommand(cmd, "press");
        }
      });
      if (!isToggle) {
        btn.addEventListener("pointerup", e => {
          e.preventDefault();
          btn.classList.remove("active");
          sendWSCommand(cmd, "release");
        });
        btn.addEventListener("pointerleave", e => {
          e.preventDefault();
          btn.classList.remove("active");
          sendWSCommand(cmd, "release");
        });
      }
    }

    window.onload = () => {
      socket = new WebSocket("ws://" + location.hostname + ":81");
      registerButton("up", "up");
      registerButton("down", "down");
      registerButton("left", "left");
      registerButton("right", "right");
      registerButton("auto", "auto", true);
    };
  </script>
</head>
<body>
  <h1>ESP32 Touch Controller</h1>
  <div class="controller">
    <div></div>
    <button class="btn" id="up"><span style="user-select: none;">Up</span></button>
    <div></div>
    <button class="btn" id="left"><span style="user-select: none;">Left</span></button>
    <div></div>
    <button class="btn" id="right"><span style="user-select: none;">Right</span></button>
    <div></div>
    <button class="btn" id="down"><span style="user-select: none;">Down</span></button>
    <div></div>
  </div>
  <button class="btn" id="auto">Auto Mode</button>
</body>
</html>
)rawliteral";

#endif
