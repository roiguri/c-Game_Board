// DOM elements
const gameBoard = document.getElementById('gameBoard');
const btnFirst = document.getElementById('btnFirst');
const btnPrev = document.getElementById('btnPrev');
const btnPlay = document.getElementById('btnPlay');
const btnPause = document.getElementById('btnPause');
const btnNext = document.getElementById('btnNext');
const btnLast = document.getElementById('btnLast');
const speedSlider = document.getElementById('speedSlider');
const stepDisplay = document.getElementById('stepDisplay');
const messageDisplay = document.getElementById('messageDisplay');
const player1Status = document.getElementById('player1Status');
const player1Shells = document.getElementById('player1Shells');
const player2Status = document.getElementById('player2Status');
const player2Shells = document.getElementById('player2Shells');

// State variables
let currentSnapshotIndex = 0;
let playIntervalId = null;
let playSpeed = parseInt(speedSlider.value);
let initialized = false;

// Direction angle mapping (in degrees)
const directionAngles = {
    [gameData.directions.UP]: 270,
    [gameData.directions.UP_RIGHT]: 315,
    [gameData.directions.RIGHT]: 0,
    [gameData.directions.DOWN_RIGHT]: 45,
    [gameData.directions.DOWN]: 90,
    [gameData.directions.DOWN_LEFT]: 135,
    [gameData.directions.LEFT]: 180,
    [gameData.directions.UP_LEFT]: 225
};

// Initialize the visualization
function initialize() {
    if (gameData.snapshots.length === 0) {
        console.error('No snapshots available for visualization');
        return;
    }
    
    // Set up game board grid
    const firstSnapshot = gameData.snapshots[0];
    const height = firstSnapshot.board.length;
    const width = height > 0 ? firstSnapshot.board[0].length : 0;
    
    gameBoard.style.gridTemplateRows = `repeat(${height}, 30px)`;
    gameBoard.style.gridTemplateColumns = `repeat(${width}, 30px)`;
    
    // Create cells
    for (let y = 0; y < height; y++) {
        for (let x = 0; x < width; x++) {
            const cell = document.createElement('div');
            cell.className = 'cell';
            cell.dataset.x = x;
            cell.dataset.y = y;
            gameBoard.appendChild(cell);
        }
    }
    
    // Add event listeners
    btnFirst.addEventListener('click', goToFirst);
    btnPrev.addEventListener('click', goToPrevious);
    btnPlay.addEventListener('click', startPlayback);
    btnPause.addEventListener('click', pausePlayback);
    btnNext.addEventListener('click', goToNext);
    btnLast.addEventListener('click', goToLast);
    speedSlider.addEventListener('input', updatePlaybackSpeed);
    
    // Initialize visualization
    renderSnapshot(currentSnapshotIndex);
    initialized = true;
}

// Render a specific snapshot
function renderSnapshot(index) {
    if (index < 0 || index >= gameData.snapshots.length) {
        console.error(`Invalid snapshot index: ${index}`);
        return;
    }
    
    const snapshot = gameData.snapshots[index];
    
    // Update status displays
    stepDisplay.textContent = `Step: ${snapshot.step} / ${gameData.snapshots[gameData.snapshots.length - 1].step}`;
    messageDisplay.textContent = snapshot.message;
    
    // Clear previous state
    const cells = gameBoard.querySelectorAll('.cell');
    cells.forEach(cell => {
        cell.className = 'cell';
        cell.innerHTML = '';
    });
    
    // Render board
    const board = snapshot.board;
    for (let y = 0; y < board.length; y++) {
        for (let x = 0; x < board[y].length; x++) {
            const cellType = board[y][x];
            const cell = getCellAt(x, y);
            
            if (!cell) continue;
            
            switch (cellType) {
                case gameData.cellTypes.WALL:
                    cell.classList.add('wall');
                    break;
                case gameData.cellTypes.MINE:
                    cell.classList.add('mine');
                    break;
                case gameData.cellTypes.TANK1:
                case gameData.cellTypes.TANK2:
                    // Tanks are rendered separately
                    break;
            }
        }
    }
    
    // Render tanks
    snapshot.tanks.forEach(tank => {
        const cell = getCellAt(tank.position.x, tank.position.y);
        if (!cell) return;
        
        if (!tank.destroyed) {
            cell.classList.add(tank.playerId === 1 ? 'tank1' : 'tank2');
            
            // Add cannon
            const cannon = document.createElement('div');
            cannon.className = 'cannon';
            cannon.style.transform = `rotate(${directionAngles[tank.direction]}deg)`;
            cell.appendChild(cannon);
        }
        
        // Update player stats
        if (tank.playerId === 1) {
            player1Status.textContent = tank.destroyed ? 'Destroyed' : 'Active';
            player1Shells.textContent = tank.remainingShells;
        } else if (tank.playerId === 2) {
            player2Status.textContent = tank.destroyed ? 'Destroyed' : 'Active';
            player2Shells.textContent = tank.remainingShells;
        }
    });
    
    // Render shells
    snapshot.shells.forEach(shell => {
        if (shell.destroyed) return;
        
        const cell = getCellAt(shell.position.x, shell.position.y);
        if (!cell) return;
        
        const shellElement = document.createElement('div');
        shellElement.className = `shell shell${shell.playerId}`;
        cell.appendChild(shellElement);
    });
    
    // Update button states
    btnFirst.disabled = index === 0;
    btnPrev.disabled = index === 0;
    btnNext.disabled = index === gameData.snapshots.length - 1;
    btnLast.disabled = index === gameData.snapshots.length - 1;
    
    currentSnapshotIndex = index;
}

// Helper to get a cell at specific coordinates
function getCellAt(x, y) {
    return gameBoard.querySelector(`[data-x="${x}"][data-y="${y}"]`);
}

// Navigation functions
function goToFirst() {
    pausePlayback();
    renderSnapshot(0);
}

function goToPrevious() {
    pausePlayback();
    if (currentSnapshotIndex > 0) {
        renderSnapshot(currentSnapshotIndex - 1);
    }
}

function goToNext() {
    pausePlayback();
    if (currentSnapshotIndex < gameData.snapshots.length - 1) {
        renderSnapshot(currentSnapshotIndex + 1);
    }
}

function goToLast() {
    pausePlayback();
    renderSnapshot(gameData.snapshots.length - 1);
}

// Playback functions
function startPlayback() {
    if (playIntervalId !== null) return;
    
    btnPlay.style.display = 'none';
    btnPause.style.display = 'inline-block';
    
    playIntervalId = setInterval(() => {
        if (currentSnapshotIndex < gameData.snapshots.length - 1) {
            renderSnapshot(currentSnapshotIndex + 1);
        } else {
            pausePlayback();
        }
    }, playSpeed);
}

function pausePlayback() {
    if (playIntervalId === null) return;
    
    clearInterval(playIntervalId);
    playIntervalId = null;
    
    btnPause.style.display = 'none';
    btnPlay.style.display = 'inline-block';
}

function updatePlaybackSpeed() {
    playSpeed = parseInt(speedSlider.value);
    
    // If currently playing, restart with new speed
    if (playIntervalId !== null) {
        pausePlayback();
        startPlayback();
    }
}

// Initialize on page load
document.addEventListener('DOMContentLoaded', initialize);

// If document already loaded, initialize immediately
if (document.readyState === 'complete' || document.readyState === 'interactive') {
    if (!initialized) {
        initialize();
    }
}