/**
 * Tank Simulator UI - Frontend JavaScript
 * Handles dynamic configuration and simulation control
 */

class SimulatorUI {
    constructor() {
        this.isRunning = false;
        this.currentMode = 'basic';
        
        this.initializeElements();
        this.bindEvents();
        this.updateUI();
        this.loadConfiguration();
    }

    initializeElements() {
        // Control elements
        this.startBtn = document.getElementById('startSimulation');
        this.stopBtn = document.getElementById('stopSimulation');
        this.statusDiv = document.getElementById('status');
        this.statusIcon = this.statusDiv.querySelector('.status-icon');
        this.statusText = this.statusDiv.querySelector('.status-text');
        
        // Configuration elements
        this.modeRadios = document.querySelectorAll('input[name="mode"]');
        this.configSummary = document.getElementById('configSummary');
        this.configText = document.getElementById('configText');
        
        // Mode config sections
        this.basicConfig = document.getElementById('basicConfig');
        this.comparativeConfig = document.getElementById('comparativeConfig');
        this.competitionConfig = document.getElementById('competitionConfig');
        
        // Output elements
        this.outputLog = document.getElementById('outputLog');
        this.clearBtn = document.getElementById('clearOutput');
        
        // Tab elements
        this.tabButtons = document.querySelectorAll('.tab-button');
        this.tabPanels = document.querySelectorAll('.tab-panel');
        
        // Results elements
        this.refreshResultsBtn = document.getElementById('refreshResults');
        this.resultsStatus = document.getElementById('resultsStatus');
        this.resultsDisplay = document.getElementById('resultsDisplay');
        
        console.log('✅ All UI elements initialized');
    }

    bindEvents() {
        // Simulation control
        this.startBtn.addEventListener('click', () => this.startSimulation());
        this.stopBtn.addEventListener('click', () => this.stopSimulation());
        
        // Mode switching
        this.modeRadios.forEach(radio => {
            radio.addEventListener('change', (e) => {
                if (e.target.checked) {
                    this.switchMode(e.target.value);
                }
            });
        });
        
        // Configuration change listeners
        const inputs = document.querySelectorAll('#configurationTab input');
        inputs.forEach(input => {
            input.addEventListener('input', () => this.updateConfigurationSummary());
            input.addEventListener('change', () => this.updateConfigurationSummary());
        });
        
        // Output controls
        this.clearBtn.addEventListener('click', () => this.clearOutput());
        
        // Tab switching
        this.tabButtons.forEach(button => {
            button.addEventListener('click', (e) => {
                const tabName = e.target.dataset.tab;
                this.switchTab(tabName);
            });
        });
        
        // Results controls
        this.refreshResultsBtn.addEventListener('click', () => this.refreshResults());
        
        console.log('✅ Event listeners bound');
    }

    switchMode(mode) {
        console.log(`🔄 Switching to ${mode} mode`);
        this.currentMode = mode;
        
        this.basicConfig.style.display = 'none';
        this.comparativeConfig.style.display = 'none';
        this.competitionConfig.style.display = 'none';
        
        document.getElementById(`${mode}Config`).style.display = 'block';
        
        this.updateConfigurationSummary();
        this.saveConfiguration();
    }

    buildConfiguration() {
        const config = {
            mode: this.currentMode,
            verbose: document.getElementById('verbose').checked
        };

        switch (this.currentMode) {
            case 'basic':
                config.gameMap = document.getElementById('gameMap').value;
                config.gameManager = document.getElementById('gameManager').value;
                config.algorithm1 = document.getElementById('algorithm1').value;
                config.algorithm2 = document.getElementById('algorithm2').value;
                break;
                
            case 'comparative':
                config.gameMap = document.getElementById('compGameMap').value;
                config.gameManagersFolder = document.getElementById('gameManagersFolder').value;
                config.algorithm1 = document.getElementById('compAlgorithm1').value;
                config.algorithm2 = document.getElementById('compAlgorithm2').value;
                if (document.getElementById('compNumThreads').value) config.numThreads = parseInt(document.getElementById('compNumThreads').value);
                break;
                
            case 'competition':
                config.gameMapsFolder = document.getElementById('gameMapsFolder').value;
                config.gameManager = document.getElementById('compGameManager').value;
                config.algorithmsFolder = document.getElementById('algorithmsFolder').value;
                if (document.getElementById('compCompNumThreads').value) config.numThreads = parseInt(document.getElementById('compCompNumThreads').value);
                break;
        }

        return config;
    }

    updateConfigurationSummary() {
        const config = this.buildConfiguration();
        let command = `../Simulator/simulator_318835816_211314471 -${config.mode}`;

        Object.entries(config).forEach(([key, value]) => {
            if (key !== 'mode' && key !== 'verbose' && value) {
                command += ` ${key}="${value}"`;
            }
        });
        
        if (config.verbose) command += ' -verbose';
        
        this.configText.textContent = command;
        this.configSummary.style.display = 'block';
    }

    setModeAndConfig(mode, config) {
        const modeRadio = document.querySelector(`input[name="mode"][value="${mode}"]`);
        if (modeRadio) {
            modeRadio.checked = true;
            this.switchMode(mode);
        }
        
        Object.entries(config).forEach(([key, value]) => {
            const el = document.getElementById(key);
            if (el && key !== 'mode') {
                if (el.type === 'checkbox') el.checked = value;
                else el.value = value;
            }
        });

        this.updateConfigurationSummary();
    }

    async startSimulation() {
        console.log('🚀 Starting simulation');
        try {
            this.setStatus('running', '🟡', 'Validating...');
            this.startBtn.disabled = true;
            
            const config = this.buildConfiguration();
            
            const response = await fetch('/api/simulate/run', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(config)
            });

            const result = await response.json();
            
            if (response.ok) {
                this.isRunning = true;
                this.stopBtn.disabled = false;
                this.setStatus('running', '🟡', `Running ${config.mode}...`);
                this.addOutputLine(`✅ ${result.message}`);
                if (result.command) this.addOutputLine(`🖥️ Cmd: ${result.command}`);
                this.startStreaming();
            } else {
                throw new Error(result?.details || result?.error || `HTTP ${response.status}`);
            }
            
        } catch (error) {
            console.error('❌ Simulation error:', error);
            this.setStatus('error', '🔴', `Error: ${error.message}`);
            this.addOutputLine(`❌ Error: ${error.message}`);
            this.startBtn.disabled = false;
        }
    }

    stopSimulation() {
        this.isRunning = false;
        this.setStatus('idle', '⚪', 'Stopped');
        this.addOutputLine('🛑 Simulation stopped by user');
        this.updateUI();
    }

    startStreaming() {
        this.addOutputLine('📡 Monitoring output...');
        this.pollOutput();
    }

    async pollOutput() {
        while (this.isRunning) {
            try {
                const response = await fetch('/api/output');
                const data = await response.json();
                
                if (data.output) {
                    data.output.trim().split('\n').forEach(line => {
                        if (line.trim()) this.addOutputLine(line.trim());
                    });
                }
                
                if (!data.running && this.isRunning) {
                    this.addOutputLine('✅ Simulation completed');
                    this.setStatus('success', '🟢', 'Completed');
                    this.isRunning = false;
                    this.updateUI();
                    break;
                }
                
                await new Promise(resolve => setTimeout(resolve, 500));
                
            } catch (error) {
                this.addOutputLine('❌ Output error: ' + error.message);
                await new Promise(resolve => setTimeout(resolve, 1000));
            }
        }
    }

    setStatus(type, icon, text) {
        this.statusDiv.className = `status status-${type}`;
        this.statusIcon.textContent = icon;
        this.statusText.textContent = text;
    }

    addOutputLine(line) {
        const timestamp = new Date().toLocaleTimeString();
        this.outputLog.textContent += `[${timestamp}] ${line}\n`;
        this.outputLog.scrollTop = this.outputLog.scrollHeight;
    }

    clearOutput() {
        this.outputLog.textContent = 'Output cleared.\n';
    }

    updateUI() {
        this.startBtn.disabled = this.isRunning;
        this.stopBtn.disabled = !this.isRunning;
        
        if (!this.isRunning && this.statusText.textContent.includes('Running')) {
            this.setStatus('idle', '⚪', 'Ready');
        }
    }

    switchTab(tabName) {
        this.tabButtons.forEach(b => b.classList.toggle('active', b.dataset.tab === tabName));
        this.tabPanels.forEach(p => p.classList.toggle('active', p.id === `${tabName}Tab`));
        if (tabName === 'results') this.refreshResults();
    }

    async refreshResults() {
        try {
            this.setResultsStatus('🔄', 'Loading...');
            const response = await fetch(`/api/results/latest?mode=${this.currentMode}`);
            if (!response.ok) throw new Error(`HTTP ${response.status}`);
            const data = await response.json();
            
            if (data.success && data.hasResults) {
                this.setResultsStatus('✅', `Results from ${data.filename} (${data.timestamp})`);
                this.displayCustomResults(data.content, this.currentMode);
            } else {
                this.setResultsStatus('ℹ️', data.message || 'No results');
                this.resultsDisplay.innerHTML = '<p class="no-results">No results available</p>';
            }
        } catch (error) {
            this.setResultsStatus('❌', 'Error loading results');
            this.resultsDisplay.innerHTML = `<p class="error">Error: ${error.message}</p>`;
        }
    }

    setResultsStatus(icon, text) {
        this.resultsStatus.querySelector('.status-icon').textContent = icon;
        this.resultsStatus.querySelector('.status-text').textContent = text;
    }

    escapeHtml(unsafe) {
        return unsafe.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;").replace(/'/g, "&#039;");
    }

    saveConfiguration() {
        const config = this.buildConfiguration();
        localStorage.setItem('tankSimulatorConfig', JSON.stringify(config));
    }

    loadConfiguration() {
        try {
            const saved = localStorage.getItem('tankSimulatorConfig');
            if (saved) {
                const config = JSON.parse(saved);
                this.setModeAndConfig(config.mode, config);
            }
        } catch (error) {
            console.log('⚠️ Could not load saved configuration');
        }
        this.updateConfigurationSummary();
    }

    displayCustomResults(content, mode) {
        try {
            if (mode === 'competition') {
                this.displayCompetitionResults(content);
            } else if (mode === 'comparative') {
                this.displayComparativeResults(content);
            } else if (mode === 'basic') {
                if (this.isValidBasicOutput(content)) {
                    this.displayBasicResults(content);
                } else {
                    this.displayBasicError(content);
                }
            } else {
                // Fallback to raw text for unknown modes
                this.resultsDisplay.innerHTML = `<pre>${this.escapeHtml(content)}</pre>`;
            }
        } catch (error) {
            console.error('Error parsing results:', error);
            // Fallback to raw text if parsing fails
            this.resultsDisplay.innerHTML = `<pre>${this.escapeHtml(content)}</pre>`;
        }
    }

    displayCompetitionResults(content) {
        const parsed = this.parseCompetitionResults(content);
        
        let html = '<div class="competition-results">';
        
        // Configuration header
        if (parsed.config.gameMapsFolder || parsed.config.gameManager) {
            html += '<div class="result-config">';
            html += '<h3>🎯 Tournament Configuration</h3>';
            if (parsed.config.gameMapsFolder) {
                html += `<p><strong>Game Maps:</strong> ${parsed.config.gameMapsFolder}</p>`;
            }
            if (parsed.config.gameManager) {
                html += `<p><strong>Game Manager:</strong> ${parsed.config.gameManager}</p>`;
            }
            html += '</div>';
        }
        
        // Leaderboard
        html += '<div class="leaderboard">';
        html += '<h3>🏆 Final Rankings</h3>';
        html += '<div class="leaderboard-table">';
        html += '<div class="leaderboard-header">';
        html += '<span class="rank-col">Rank</span>';
        html += '<span class="algorithm-col">Algorithm</span>';
        html += '<span class="score-col">Score</span>';
        html += '</div>';
        
        parsed.results.forEach((result, index) => {
            const rankClass = index < 3 ? `rank-${index + 1}` : '';
            const medal = index === 0 ? '🥇' : index === 1 ? '🥈' : index === 2 ? '🥉' : '';
            
            html += `<div class="leaderboard-row ${rankClass}">`;
            html += `<span class="rank-col">${medal} #${index + 1}</span>`;
            html += `<span class="algorithm-col">${this.escapeHtml(result.algorithm)}</span>`;
            html += `<span class="score-col">${result.score}</span>`;
            html += '</div>';
        });
        
        html += '</div>';
        html += '</div>';
        html += '</div>';
        
        this.resultsDisplay.innerHTML = html;
    }

    parseCompetitionResults(content) {
        const lines = content.trim().split('\n');
        const results = [];
        const config = {};
        
        // Parse configuration lines
        for (const line of lines) {
            const trimmed = line.trim();
            if (trimmed.startsWith('game_maps_folder=')) {
                config.gameMapsFolder = trimmed.substring('game_maps_folder='.length);
            } else if (trimmed.startsWith('game_manager=')) {
                config.gameManager = trimmed.substring('game_manager='.length);
            } else if (trimmed && !trimmed.includes('=')) {
                // This should be an algorithm result line
                const parts = trimmed.split(' ');
                if (parts.length >= 2) {
                    const score = parseInt(parts[parts.length - 1]);
                    const algorithm = parts.slice(0, -1).join(' ');
                    if (!isNaN(score)) {
                        results.push({ algorithm, score });
                    }
                }
            }
        }
        
        return { config, results };
    }

    displayComparativeResults(content) {
        const parsed = this.parseComparativeResults(content);
        
        // Try to get map dimensions for better formatting
        this.loadMapDimensions(parsed.config.gameMap);
        
        let html = '<div class="comparative-results">';
        
        // Configuration header
        if (parsed.config.gameMap || parsed.config.algorithm1 || parsed.config.algorithm2) {
            html += '<div class="result-config">';
            html += '<h3>🎯 Comparative Analysis Configuration</h3>';
            if (parsed.config.gameMap) {
                html += `<p><strong>Game Map:</strong> ${parsed.config.gameMap}</p>`;
            }
            if (parsed.config.algorithm1) {
                html += `<p><strong>Algorithm 1:</strong> ${parsed.config.algorithm1}</p>`;
            }
            if (parsed.config.algorithm2) {
                html += `<p><strong>Algorithm 2:</strong> ${parsed.config.algorithm2}</p>`;
            }
            html += '</div>';
        }
        
        // Result Groups
        html += '<div class="result-groups">';
        html += '<h3>📊 Results by Frequency</h3>';
        html += '<p class="frequency-note">Groups ordered from most common to least common outcome</p>';
        
        parsed.groups.forEach((group, index) => {
            const isFrequent = index === 0;
            const cardClass = isFrequent ? 'result-card most-frequent' : 'result-card';
            
            html += `<div class="${cardClass}">`;
            html += '<div class="card-header">';
            html += `<h4>Group ${index + 1} ${isFrequent ? '(Most Common)' : ''}</h4>`;
            html += '<div class="game-managers">';
            
            // Display GameManager names as badges
            group.gameManagers.forEach(gm => {
                html += `<span class="gm-badge">${this.escapeHtml(this.extractManagerName(gm))}</span>`;
            });
            
            html += '</div>';
            html += '</div>';
            
            html += '<div class="game-result">';
            html += `<div class="outcome"><strong>Outcome:</strong> ${this.escapeHtml(group.outcome)}</div>`;
            html += `<div class="rounds"><strong>Rounds:</strong> ${group.rounds}</div>`;
            html += '</div>';
            
            // Board state
            if (group.boardState && group.boardState.trim()) {
                html += '<div class="board-state">';
                html += '<div class="board-header">';
                html += '<h5>Final Board State:</h5>';
                html += '<div class="board-legend">';
                html += '<span class="legend-item"><span class="symbol wall">#</span> Wall</span>';
                html += '<span class="legend-item"><span class="symbol mine">@</span> Mine</span>';
                html += '<span class="legend-item"><span class="symbol player1">1</span> Player 1</span>';
                html += '<span class="legend-item"><span class="symbol player2">2</span> Player 2</span>';
                html += '<span class="legend-item"><span class="symbol shell">*</span> Shell</span>';
                html += '</div>';
                html += '</div>';
                html += '<div class="board-grid">';
                html += this.formatBoardState(group.boardState);
                html += '</div>';
                html += '</div>';
            }
            
            html += '</div>';
        });
        
        html += '</div>';
        html += '</div>';
        
        this.resultsDisplay.innerHTML = html;
    }

    parseComparativeResults(content) {
        const lines = content.split('\n');
        const config = {};
        const groups = [];
        
        let i = 0;
        
        // Parse configuration section
        while (i < lines.length && lines[i].trim() !== '') {
            const line = lines[i].trim();
            if (line.startsWith('game_map=')) {
                config.gameMap = line.substring('game_map='.length);
            } else if (line.startsWith('algorithm1=')) {
                config.algorithm1 = line.substring('algorithm1='.length);
            } else if (line.startsWith('algorithm2=')) {
                config.algorithm2 = line.substring('algorithm2='.length);
            }
            i++;
        }
        
        // Skip empty line after config
        while (i < lines.length && lines[i].trim() === '') {
            i++;
        }
        
        // Parse result groups
        while (i < lines.length) {
            const group = {
                gameManagers: [],
                outcome: '',
                rounds: 0,
                boardState: ''
            };
            
            // Parse GameManager line(s) - could be comma-separated
            const gmLine = lines[i].trim();
            if (gmLine) {
                if (gmLine.includes(',')) {
                    // Multiple GameManagers in one line
                    group.gameManagers = gmLine.split(',').map(gm => gm.trim());
                } else {
                    // Single GameManager
                    group.gameManagers = [gmLine];
                }
            }
            i++;
            
            // Parse outcome line
            if (i < lines.length) {
                group.outcome = lines[i].trim();
                i++;
            }
            
            // Parse rounds line
            if (i < lines.length) {
                const roundsStr = lines[i].trim();
                group.rounds = parseInt(roundsStr) || 0;
                i++;
            }
            
            // Parse board state (until next GameManager or end)
            const boardLines = [];
            while (i < lines.length) {
                const line = lines[i];
                // Check if this is the start of a new group (GameManager line)
                if (line.trim() && (line.includes('.so') || line.includes('GameManager'))) {
                    break;
                }
                boardLines.push(line);
                i++;
            }
            
            group.boardState = boardLines.join('\n').trim();
            groups.push(group);
        }
        
        return { config, groups };
    }

    extractManagerName(fullPath) {
        // Extract readable name from GameManager path
        const filename = fullPath.split('/').pop();
        return filename.replace(/^lib/, '').replace(/_\d+_\d+\.so$/, '').replace(/TestGameManager_/, '');
    }

    async loadMapDimensions(mapPath) {
        if (!mapPath) return;
        
        try {
            // Convert relative path to server-accessible path
            const serverPath = mapPath.replace('../UserCommon/bonus/ui_server/', '');
            const response = await fetch(`/${serverPath}`);
            const mapContent = await response.text();
            
            const lines = mapContent.split('\n');
            for (const line of lines) {
                if (line.startsWith('Rows = ')) {
                    this.mapRows = parseInt(line.substring('Rows = '.length));
                } else if (line.startsWith('Cols = ')) {
                    this.mapCols = parseInt(line.substring('Cols = '.length));
                }
            }
        } catch (error) {
            console.log('Could not load map dimensions:', error);
            // Use fallback dimensions
            this.mapRows = 15;
            this.mapCols = 20;
        }
    }

    formatBoardState(boardState) {
        if (!boardState || !boardState.trim()) {
            return '<div class="empty-board">No board state available</div>';
        }

        // Use fallback dimensions if not loaded
        const rows = this.mapRows || 15;
        const cols = this.mapCols || 20;
        
        // Create a proper 2D grid
        const grid = Array(rows).fill().map(() => Array(cols).fill(' '));
        
        // Fill grid with board state data
        const lines = boardState.split('\n');
        lines.forEach((line, rowIndex) => {
            if (rowIndex < rows && line) {
                for (let colIndex = 0; colIndex < Math.min(line.length, cols); colIndex++) {
                    grid[rowIndex][colIndex] = line[colIndex];
                }
            }
        });
        
        // Create HTML grid
        let html = `<div class="board-container" style="grid-template-columns: repeat(${cols}, 1fr); grid-template-rows: repeat(${rows}, 1fr);">`;
        
        for (let row = 0; row < rows; row++) {
            for (let col = 0; col < cols; col++) {
                const char = grid[row][col];
                const cellClass = this.getBoardCellClass(char);
                const displayChar = char === ' ' ? '' : this.escapeHtml(char);
                
                html += `<div class="board-cell ${cellClass}" data-row="${row}" data-col="${col}">`;
                html += `<span class="cell-content">${displayChar}</span>`;
                html += '</div>';
            }
        }
        
        html += '</div>';
        
        // Add grid info
        html += `<div class="grid-info">Grid: ${rows} rows × ${cols} columns</div>`;
        
        return html;
    }

    getBoardCellClass(char) {
        switch (char) {
            case '#': return 'wall';
            case '@': return 'mine';
            case '1': return 'player1';
            case '2': return 'player2';
            case '*': return 'shell';
            case ' ': return 'empty';
            default: return 'unknown';
        }
    }

    displayBasicResults(content) {
        const parsed = this.parseBasicResults(content);
        
        let html = '<div class="basic-results">';
        
        // Configuration summary
        if (parsed.config) {
            html += '<div class="result-config">';
            html += '<h3>🎯 Basic Game Configuration</h3>';
            if (parsed.config.gameMap) {
                html += `<p><strong>Game Map:</strong> ${parsed.config.gameMap}</p>`;
            }
            if (parsed.config.gameManager) {
                html += `<p><strong>Game Manager:</strong> ${parsed.config.gameManager}</p>`;
            }
            if (parsed.config.algorithm1) {
                html += `<p><strong>Algorithm 1:</strong> ${parsed.config.algorithm1}</p>`;
            }
            if (parsed.config.algorithm2) {
                html += `<p><strong>Algorithm 2:</strong> ${parsed.config.algorithm2}</p>`;
            }
            html += '</div>';
        }
        
        // Game result summary
        html += '<div class="game-summary">';
        html += '<h3>🏆 Game Result</h3>';
        
        if (parsed.winner !== null) {
            html += '<div class="winner-section">';
            if (parsed.winner === 0) {
                html += '<div class="result-tie">🤝 TIE GAME</div>';
            } else {
                html += `<div class="result-winner">🎉 PLAYER ${parsed.winner} WINS!</div>`;
            }
            html += '</div>';
            
            if (parsed.reason) {
                html += `<div class="game-reason"><strong>Reason:</strong> ${this.escapeHtml(parsed.reason)}</div>`;
            }
            
            if (parsed.rounds !== null) {
                html += `<div class="game-rounds"><strong>Rounds Played:</strong> ${parsed.rounds}</div>`;
            }
        } else {
            html += '<div class="result-unknown">Game result not clearly determined from output</div>';
        }
        
        html += '</div>';
        
        // Final board state (if detected)
        if (parsed.boardState && parsed.boardState.trim()) {
            // Try to get map dimensions for this game
            if (parsed.config && parsed.config.gameMap) {
                this.loadMapDimensions(parsed.config.gameMap);
            }
            
            html += '<div class="board-state">';
            html += '<div class="board-header">';
            html += '<h3>🎮 Final Board State</h3>';
            html += '<div class="board-legend">';
            html += '<span class="legend-item"><span class="symbol wall">#</span> Wall</span>';
            html += '<span class="legend-item"><span class="symbol mine">@</span> Mine</span>';
            html += '<span class="legend-item"><span class="symbol player1">1</span> Player 1</span>';
            html += '<span class="legend-item"><span class="symbol player2">2</span> Player 2</span>';
            html += '<span class="legend-item"><span class="symbol shell">*</span> Shell</span>';
            html += '</div>';
            html += '</div>';
            html += '<div class="board-grid">';
            html += this.formatBoardState(parsed.boardState);
            html += '</div>';
            html += '</div>';
        }
        
        
        html += '</div>';
        
        this.resultsDisplay.innerHTML = html;
    }

    parseBasicResults(content) {
        const lines = content.split('\n');
        const result = {
            config: {},
            winner: null,
            reason: null,
            rounds: null,
            boardState: ''
        };
        
        let inBoardState = false;
        const boardLines = [];
        
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i].trim();
            
            // Extract configuration from command line
            if (line.includes('simulator_318835816_211314471') && line.includes('-basic')) {
                const configMatch = line.match(/game_map="([^"]*)".*game_manager="([^"]*)".*algorithm1="([^"]*)".*algorithm2="([^"]*)"/);
                if (configMatch) {
                    result.config.gameMap = configMatch[1];
                    result.config.gameManager = configMatch[2];
                    result.config.algorithm1 = configMatch[3];
                    result.config.algorithm2 = configMatch[4];
                }
            }
            
            // Look for winner announcements - support multiple formats
            if (line.includes('Player 1 wins') || line.includes('Player 1 Wins') || line.includes('Winner: Player 1')) {
                result.winner = 1;
            } else if (line.includes('Player 2 wins') || line.includes('Player 2 Wins') || line.includes('Winner: Player 2')) {
                result.winner = 2;
            } else if (line.toLowerCase().includes('tie') || line.toLowerCase().includes('draw')) {
                result.winner = 0;
            }
            
            // Look for game end reasons
            if (line.includes('all tanks destroyed') || line.includes('maximum steps reached') || line.includes('no shells remaining') || line.includes('Reason:')) {
                result.reason = line;
            }
            
            // Look for round count
            const roundMatch = line.match(/(?:total\s+)?rounds?\s*:?\s*(\d+)/i);
            if (roundMatch) {
                result.rounds = parseInt(roundMatch[1]);
            }
            
            // Detect board state boundaries
            if (line.includes('Final board') || line.includes('Board state') || inBoardState) {
                inBoardState = true;
                if (line.match(/^[\s#@12*]+$/)) {
                    boardLines.push(lines[i]); // Use original line with spacing
                }
            }
            
        }
        
        // Set board state if we found any
        if (boardLines.length > 0) {
            result.boardState = boardLines.join('\n');
        }
        
        return result;
    }

    isValidBasicOutput(content) {
        // Check if the output contains the expected structure for basic mode
        return content.includes('Game completed!') && content.includes('Winner:');
    }

    displayBasicError(content) {
        let html = '<div class="basic-results">';
        
        // Determine the type of error
        let errorType = 'Unknown Error';
        let errorMessage = 'The simulation did not produce the expected output format.';
        
        if (!content || content.trim() === '') {
            errorType = 'No Output';
            errorMessage = 'The simulation produced no output. This may indicate a configuration or file path error.';
        } else if (content.includes('Error') || content.includes('error')) {
            errorType = 'Simulation Error';
            errorMessage = 'The simulation encountered an error during execution.';
        } else if (content.includes('No such file') || content.includes('not found')) {
            errorType = 'File Not Found';
            errorMessage = 'One or more required files (map, game manager, or algorithms) could not be found.';
        } else if (content.includes('Permission denied')) {
            errorType = 'Permission Error';
            errorMessage = 'Permission denied accessing required files. Check file permissions.';
        } else if (!content.includes('Game completed!')) {
            errorType = 'Incomplete Execution';
            errorMessage = 'The simulation did not complete successfully or was interrupted.';
        }
        
        html += '<div class="error-summary">';
        html += `<h3>⚠️ ${errorType}</h3>`;
        html += `<p class="error-description">${errorMessage}</p>`;
        html += '</div>';
        
        // Show raw output for debugging if it's not empty
        if (content && content.trim() !== '') {
            html += '<div class="debug-output">';
            html += '<h4>Debug Information:</h4>';
            html += `<pre class="raw-output">${this.escapeHtml(content)}</pre>`;
            html += '</div>';
        }
        
        html += '</div>';
        
        this.resultsDisplay.innerHTML = html;
    }

}

document.addEventListener('DOMContentLoaded', () => {
    window.simulatorUI = new SimulatorUI();
});