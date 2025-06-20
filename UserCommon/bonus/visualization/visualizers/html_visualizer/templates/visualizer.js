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
const countdownDisplay = document.getElementById('countdownDisplay');
const actionLog = document.getElementById('actionLog');
const tankList = document.getElementById('tankList');

// State variables
let currentSnapshotIndex = 0;
let playIntervalId = null;
let playSpeed = parseInt(speedSlider.value);
let initialized = false;

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
    
    // Update current index first
    currentSnapshotIndex = index;
    
    // Update status displays
    stepDisplay.textContent = `Step: ${snapshot.step} / ${gameData.snapshots[gameData.snapshots.length - 1].step}`;
    
    // Update countdown display
    if (snapshot.countdown > 0) {
        countdownDisplay.textContent = `Countdown: ${snapshot.countdown}`;
        countdownDisplay.classList.add('active');
    } else {
        countdownDisplay.textContent = '';
        countdownDisplay.classList.remove('active');
    }
    
    // Clear previous state
    const cells = gameBoard.querySelectorAll('.cell');
    cells.forEach(cell => {
        cell.className = 'cell';
        cell.innerHTML = '';
    });

    // Create a wall health lookup map for easy access
    const wallHealthMap = {};
    if (snapshot.wallHealth) {
        snapshot.wallHealth.forEach(wall => {
            wallHealthMap[`${wall.x},${wall.y}`] = wall.health;
        });
    }
    
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
                    const key = `${x},${y}`;
                    const health = wallHealthMap[key] || 2; // Default to full health if not found
                    cell.classList.add(`wall-health-${health}`);
                    break;
                case gameData.cellTypes.MINE:
                    cell.classList.add('mine');
                    break;
                case gameData.cellTypes.TANK1:
                case gameData.cellTypes.TANK2:
                case gameData.cellTypes.TANK3:
                case gameData.cellTypes.TANK4:
                case gameData.cellTypes.TANK5:
                case gameData.cellTypes.TANK6:
                case gameData.cellTypes.TANK7:
                case gameData.cellTypes.TANK8:
                case gameData.cellTypes.TANK9:
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
            cell.classList.add(`tank${tank.playerId}`);
            
            // Add cannon
            const cannon = document.createElement('div');
            cannon.className = 'cannon';
            
            // Center coordinates (relative to cell)
            const centerX = 15;
            const centerY = 15;

            // Direction-specific styling
            switch(tank.direction) {
              case 0: // Up
                cannon.style.width = '4px';
                cannon.style.height = '15px';
                cannon.style.left = `${centerX - 2}px`; // Center the 4px width
                cannon.style.top = '0px'; // Start from top of cell
                break;
                
              case 1: // UpRight
                // For diagonal, create a longer cannon to account for the diagonal distance
                cannon.style.width = '21px'; // ~15px * √2 (diagonal)
                cannon.style.height = '4px';
                cannon.style.transformOrigin = 'left center';
                cannon.style.transform = 'rotate(-45deg)';
                cannon.style.left = `${centerX}px`;
                cannon.style.top = `${centerY - 2}px`; // Center the 4px height
                break;
                
              case 2: // Right
                cannon.style.width = '15px';
                cannon.style.height = '4px';
                cannon.style.left = `${centerX}px`; // Start from center
                cannon.style.top = `${centerY - 2}px`; // Center the 4px height
                break;
                
              case 3: // DownRight
                cannon.style.width = '21px'; // ~15px * √2 (diagonal)
                cannon.style.height = '4px';
                cannon.style.transformOrigin = 'left center';
                cannon.style.transform = 'rotate(45deg)';
                cannon.style.left = `${centerX}px`;
                cannon.style.top = `${centerY - 2}px`; // Center the 4px height
                break;
                
              case 4: // Down
                cannon.style.width = '4px';
                cannon.style.height = '15px';
                cannon.style.left = `${centerX - 2}px`; // Center the 4px width
                cannon.style.top = `${centerY}px`; // Start from center
                break;
                
              case 5: // DownLeft
                cannon.style.width = '21px'; // ~15px * √2 (diagonal)
                cannon.style.height = '4px';
                cannon.style.transformOrigin = 'right center';
                cannon.style.transform = 'rotate(-45deg)';
                cannon.style.left = `${centerX - 21}px`; // Position so right edge is at center
                cannon.style.top = `${centerY - 2}px`; // Center the 4px height
                break;
                
              case 6: // Left
                cannon.style.width = '15px';
                cannon.style.height = '4px';
                cannon.style.left = `${centerX - 15}px`; // End at center
                cannon.style.top = `${centerY - 2}px`; // Center the 4px height
                break;
                
              case 7: // UpLeft
                cannon.style.width = '21px'; // ~15px * √2 (diagonal)
                cannon.style.height = '4px';
                cannon.style.transformOrigin = 'right center';
                cannon.style.transform = 'rotate(45deg)';
                cannon.style.left = `${centerX - 21}px`; // Position so right edge is at center
                cannon.style.top = `${centerY - 2}px`; // Center the 4px height
                break;
            }
            cell.appendChild(cannon);
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
    
    // Update tank stats display
    updateTankStats(snapshot);
    
    // Update action log
    updateActionLog();
    
    // Update button states
    btnFirst.disabled = index === 0;
    btnPrev.disabled = index === 0;
    btnNext.disabled = index === gameData.snapshots.length - 1;
    btnLast.disabled = index === gameData.snapshots.length - 1;
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

// Update tank stats display
function updateTankStats(snapshot) {
    tankList.innerHTML = '';
    
    // Sort tanks by player ID for consistent ordering
    const sortedTanks = [...snapshot.tanks].sort((a, b) => a.playerId - b.playerId);
    
    sortedTanks.forEach(tank => {
        const tankDiv = document.createElement('div');
        tankDiv.className = 'tank-info';
        if (tank.destroyed) {
            tankDiv.classList.add('tank-destroyed');
        }
        
        const colorDiv = document.createElement('div');
        colorDiv.className = `tank-color tank${tank.playerId}`;
        
        const detailsDiv = document.createElement('div');
        detailsDiv.className = 'tank-details';
        detailsDiv.innerHTML = `
            <div class="tank-player">P${tank.playerId}</div>
            <div class="tank-status">${tank.destroyed ? 'Destroyed' : tank.remainingShells + ' shells'}</div>
        `;
        
        tankDiv.appendChild(colorDiv);
        tankDiv.appendChild(detailsDiv);
        tankList.appendChild(tankDiv);
    });
}

// Update action log display
function updateActionLog() {
    // Clear the log
    actionLog.innerHTML = '';
    
    const currentSnapshot = gameData.snapshots[currentSnapshotIndex];
    
    // If we're at the last step, try to show game result
    if (currentSnapshotIndex === gameData.snapshots.length - 1) {
        // Check all snapshots for any result-like message
        for (let i = gameData.snapshots.length - 1; i >= 0; i--) {
            const snap = gameData.snapshots[i];
            
            if (snap.message && snap.message.trim() !== '' && 
                snap.message !== `Step ${snap.step}`) {
                // This is a meaningful message, use it as the result
                const logEntry = document.createElement('div');
                logEntry.innerHTML = `<strong>Game Complete:</strong> ${snap.message}`;
                actionLog.appendChild(logEntry);
                return;
            }
        }
        
        // If no meaningful message found, show basic completion
        const logEntry = document.createElement('div');
        logEntry.innerHTML = `<strong>Game Complete</strong>`;
        actionLog.appendChild(logEntry);
        return;
    }
    
    // Show only the current snapshot's message if it's meaningful
    if (currentSnapshot && currentSnapshot.message && 
        currentSnapshot.message !== '' && 
        currentSnapshot.message !== `Step ${currentSnapshot.step}` &&
        currentSnapshot.message.trim() !== '') {
        
        const logEntry = document.createElement('div');
        logEntry.innerHTML = `<strong>Step ${currentSnapshot.step}:</strong> ${currentSnapshot.message}`;
        actionLog.appendChild(logEntry);
    }
}

// If document already loaded, initialize immediately
if (document.readyState === 'complete' || document.readyState === 'interactive') {
    if (!initialized) {
        initialize();
    }
}