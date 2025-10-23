document.addEventListener('DOMContentLoaded', () => {
    // Views
    const mainMenu = document.getElementById('main-menu');
    const rcView = document.getElementById('rc-view');

    // Buttons
    const rcModeBtn = document.getElementById('rc-mode-btn');
    const loadMazeBtn = document.getElementById('load-maze-btn');
    const solveMazeBtn = document.getElementById('solve-maze-btn');
    const lineFollowingBtn = document.getElementById('line-following-btn');
    const rcBackBtn = document.getElementById('rc-back-btn');

    // Modals
    const loadMazeModal = document.getElementById('load-maze-modal');
    const solveMazeModal = document.getElementById('solve-maze-modal');

    // Modal components
    const mazeList = document.getElementById('maze-list');
    const loadSelectedMazeBtn = document.getElementById('load-selected-maze-btn');
    const mazeNameInput = document.getElementById('maze-name-input');
    const startSolvingBtn = document.getElementById('start-solving-btn');
    const loader = document.querySelector('#solve-maze-modal .loader');

    // Close buttons
    const closeBtns = document.querySelectorAll('.close-btn');

    // --- WebSocket Communication ---
    let websocket;

    const initWebSocket = () => {
        // Replace with your ESP32's IP address
        websocket = new WebSocket('ws://192.168.4.1/ws');

        websocket.onopen = () => {
            console.log('WebSocket connection established');
        };

        websocket.onmessage = (event) => {
            console.log('Message from server: ', event.data);
            const [type, data] = event.data.split(/:(.*)/s);
            if (type === 'maze_solution') {
                const [name, solution] = data.split(';');
                saveMaze(name, solution);
                alert(`Maze "${name}" solved and saved!`);
                resetSolveMazeModal();
            } else if (type === 'maze_fail') {
                alert(`Failed to solve maze: ${data}`);
                resetSolveMazeModal();
            }
        };

        websocket.onclose = () => {
            console.log('WebSocket connection closed');
            // Optionally, try to reconnect
            setTimeout(initWebSocket, 2000);
        };

        websocket.onerror = (error) => {
            console.error('WebSocket error: ', error);
        };
    };

    const sendMessage = (message) => {
        if (websocket.readyState === WebSocket.OPEN) {
            websocket.send(message);
        } else {
            console.error('WebSocket is not connected.');
        }
    };

    // Initialize WebSocket on page load
    initWebSocket();

    const resetSolveMazeModal = () => {
        solveMazeModal.style.display = 'none';
        loader.classList.add('hidden');
        mazeNameInput.classList.remove('hidden');
        startSolvingBtn.classList.remove('hidden');
        mazeNameInput.value = '';
    };

    // --- Maze Storage ---
    const getMazes = () => {
        const mazes = localStorage.getItem('mazes');
        return mazes ? JSON.parse(mazes) : [];
    };

    const saveMaze = (name, solution) => {
        const mazes = getMazes();
        mazes.push({ name, solution });
        localStorage.setItem('mazes', JSON.stringify(mazes));
    };

    const populateMazeList = () => {
        mazeList.innerHTML = '';
        const mazes = getMazes();
        mazes.forEach(maze => {
            const mazeItem = document.createElement('div');
            mazeItem.classList.add('maze-item');
            mazeItem.textContent = maze.name;
            mazeItem.addEventListener('click', () => {
                const currentSelected = document.querySelector('.maze-item.selected');
                if (currentSelected) {
                    currentSelected.classList.remove('selected');
                }
                mazeItem.classList.add('selected');
            });
            mazeList.appendChild(mazeItem);
        });
    };

    // --- Joystick ---
    const createJoystick = () => {
        const options = {
            zone: document.getElementById('joystick-container'),
            mode: 'static',
            position: { left: '50%', top: '50%' },
            color: '#00f5d4',
            size: 150
        };
        const manager = nipplejs.create(options);

        manager.on('move', (evt, data) => {
            const x = Math.round(Math.cos(data.angle.radian) * data.distance / (options.size / 2) * 100);
            const y = Math.round(Math.sin(data.angle.radian) * data.distance / (options.size / 2) * 100);
            sendMessage(`rc:${x},${y}`);
        });

        manager.on('end', () => {
            sendMessage('rc:0,0');
        });
    };


    // --- Event Listeners ---
    rcModeBtn.addEventListener('click', () => {
        mainMenu.classList.add('hidden');
        rcView.classList.remove('hidden');
        createJoystick();
    });

    rcBackBtn.addEventListener('click', () => {
        mainMenu.classList.remove('hidden');
        rcView.classList.add('hidden');
    });

    loadMazeBtn.addEventListener('click', () => {
        populateMazeList();
        loadMazeModal.style.display = 'block';
    });

    solveMazeBtn.addEventListener('click', () => {
        solveMazeModal.style.display = 'block';
    });

    lineFollowingBtn.addEventListener('click', () => {
        sendMessage('start_line_following');
    });

    startSolvingBtn.addEventListener('click', () => {
        const mazeName = mazeNameInput.value.trim();
        if (mazeName) {
            sendMessage(`start_solving:${mazeName}`);
            mazeNameInput.classList.add('hidden');
            startSolvingBtn.classList.add('hidden');
            loader.classList.remove('hidden');
        } else {
            alert('Please enter a name for the maze.');
        }
    });

    loadSelectedMazeBtn.addEventListener('click', () => {
        const selectedMazeItem = document.querySelector('.maze-item.selected');
        if (selectedMazeItem) {
            const mazeName = selectedMazeItem.textContent;
            const mazes = getMazes();
            const maze = mazes.find(m => m.name === mazeName);
            if (maze) {
                sendMessage(`load_maze:${maze.solution}`);
                alert(`Sent maze "${maze.name}" to robot.`);
                loadMazeModal.style.display = 'none';
            }
        } else {
            alert('Please select a maze to load.');
        }
    });

    // --- Modal Closing Logic ---
    closeBtns.forEach(btn => {
        btn.addEventListener('click', () => {
            loadMazeModal.style.display = 'none';
            solveMazeModal.style.display = 'none';
        });
    });

    window.addEventListener('click', (event) => {
        if (event.target == loadMazeModal) {
            loadMazeModal.style.display = 'none';
        }
        if (event.target == solveMazeModal) {
            solveMazeModal.style.display = 'none';
        }
    });
});
