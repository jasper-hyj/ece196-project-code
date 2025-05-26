const char index_html[] = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>ESP32 Bot Controller</title>
    <style>
      body {
        text-align: center;
        font-family: sans-serif;
        background-color: #f0f0f0;
      }
      canvas {
        background: #3498db;
        border-radius: 16px;
        margin-top: 20px;
        touch-action: none;
      }
    </style>
    <script>
      let socket;
      let canvas, ctx;
      const size = 300;
      let waypoints = [];

      // Bot position and movement state
      let bot = { x: 0, y: 0 };
      let currentTargetIndex = 0;
      let moving = false;
      let moveSpeed = 0.5; // units per frame (adjust for speed)
      let dashOffset = 0;

      function createGradientPath(points) {
        // Create a path that goes through all given points (in canvas coords)
        const path = new Path2D();
        if (points.length === 0) return path;
        path.moveTo(points[0].x, points[0].y);
        for (let i = 1; i < points.length; i++) {
          path.lineTo(points[i].x, points[i].y);
        }
        return path;
      }

      function draw() {
        ctx.clearRect(0, 0, size, size);

        // Draw dotted path from current bot position to remaining waypoints
        if (waypoints.length > 0 && currentTargetIndex < waypoints.length) {
          const points = [];
          points.push({ x: (bot.x / 100) * size, y: (bot.y / 100) * size });
          for (let i = currentTargetIndex; i < waypoints.length; i++) {
            points.push({
              x: (waypoints[i].x / 100) * size,
              y: (waypoints[i].y / 100) * size,
            });
          }

          const path = createGradientPath(points); // still returns a Path2D

          ctx.lineWidth = 2;
          ctx.strokeStyle = "#000000"; // solid black for contrast
          ctx.setLineDash([6, 6]); // dashed pattern: 6px dash, 6px gap
          ctx.lineDashOffset = -dashOffset; // animate dash offset for motion
          ctx.lineCap = "round";
          ctx.lineJoin = "round";
          ctx.stroke(path);
          ctx.setLineDash([]); // reset dash after path is drawn
        }

        // Draw bot as a red circle
        ctx.fillStyle = "#c0392b";
        ctx.beginPath();
        ctx.arc((bot.x / 100) * size, (bot.y / 100) * size, 8, 0, 2 * Math.PI);
        ctx.fill();

        // Draw arrow if moving
        if (moving && currentTargetIndex < waypoints.length) {
          const target = waypoints[currentTargetIndex];
          const dx = target.x - bot.x;
          const dy = target.y - bot.y;
          const angle = Math.atan2(dy, dx);
          const arrowLength = 16;
          const arrowWidth = 6;
          const startX = (bot.x / 100) * size;
          const startY = (bot.y / 100) * size;
          const tipX = startX + Math.cos(angle) * arrowLength;
          const tipY = startY + Math.sin(angle) * arrowLength;
          const leftX = startX + Math.cos(angle + Math.PI / 2) * arrowWidth;
          const leftY = startY + Math.sin(angle + Math.PI / 2) * arrowWidth;
          const rightX = startX + Math.cos(angle - Math.PI / 2) * arrowWidth;
          const rightY = startY + Math.sin(angle - Math.PI / 2) * arrowWidth;

          ctx.fillStyle = "#c0392b";
          ctx.beginPath();
          ctx.moveTo(tipX, tipY);
          ctx.lineTo(leftX, leftY);
          ctx.lineTo(rightX, rightY);
          ctx.closePath();
          ctx.fill();
        }
      }

      function sendTarget(x, y) {
        if (socket.readyState === WebSocket.OPEN) {
          socket.send(JSON.stringify({ x, y }));
        } else {
          console.warn("WebSocket not open yet, cannot send");
        }
      }

      window.onload = () => {
        canvas = document.getElementById("touchCanvas");
        ctx = canvas.getContext("2d");

        canvas.addEventListener("pointerdown", (e) => {
          const rect = canvas.getBoundingClientRect();
          const x = Math.round(((e.clientX - rect.left) / size) * 100);
          const y = Math.round(((e.clientY - rect.top) / size) * 100);
          waypoints.push({ x, y });
          sendTarget(x, y);
          draw();
        });

        socket = new WebSocket("ws://" + location.hostname + ":81");

        socket.onmessage = (event) => {
          const data = JSON.parse(event.data);
          if (data.x !== undefined && data.y !== undefined) {
            bot.x = data.x;
            bot.y = data.y;
            while (
              currentTargetIndex < waypoints.length &&
              Math.hypot(
                waypoints[currentTargetIndex].x - bot.x,
                waypoints[currentTargetIndex].y - bot.y
              ) < 5
            ) {
              currentTargetIndex++;
            }
            draw();
          }
        };

        // Animation loop
        function animate() {
          dashOffset = (dashOffset + 1) % 12; // animate dash movement
          draw();
          requestAnimationFrame(animate);
        }

        animate();
        draw();
      };
    </script>
  </head>
  <body>
    <h1>ESP32 Bot Controller</h1>
    <canvas id="touchCanvas" width="300" height="300"></canvas>
  </body>
</html>

)rawliteral";
