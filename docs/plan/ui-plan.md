# **Tank Simulator UI: Design Document**

This document outlines the design and architecture for a lightweight, web-based UI for the Tank Simulator, built directly into the existing C++ project.

### **1\. Core Architecture**

The UI will be implemented as a **self-contained C++ web server** that runs as a separate executable (tank\_simulator\_ui). This server will:

1. **Serve a Web Interface:** Provide a clean, browser-based UI built with vanilla HTML, CSS, and JavaScript.  
2. **Execute the Simulator:** Launch the existing simulator executable as a child process.  
3. **Stream Real-time Output:** Capture the stdout and stderr from the simulator and stream it live to the web interface.  
4. **Manage Dependencies with CMake:** All required libraries will be downloaded and managed by CMake, ensuring a seamless build process with no external toolchains (like Node.js) required.

This single-executable approach is lean, fast, and keeps the entire project within the C++ ecosystem.

### **2\. Technical Stack**

* **Backend:** C++  
  * **Web Server Library:** **cpp-httplib** \- A header-only, cross-platform library that is extremely easy to integrate with CMake.  
  * **Process Management:** **reproc** \- A robust, cross-platform C++ library for starting and managing child processes, offering superior control over I/O compared to standard library functions.  
* **Frontend:**  
  * **HTML5** for structure.  
  * **CSS3** for styling.  
  * **Vanilla JavaScript (ES6+)** for interactivity.  
* **Build System:** **CMake** will manage all dependencies using FetchContent.

### **3\. Key Components**

#### **C++ Backend (tank\_simulator\_ui executable)**

* **ui\_main.cpp**: The main entry point. Initializes and starts the httplib server.  
* **HTTP Endpoints:**  
  * GET /: Serves the main index.html file and its assets (CSS, JS).  
  * GET /api/files/scan?path=\<directory\>: Scans a given directory on the server for files (e.g., maps, algorithms) and returns a JSON list.  
  * POST /api/simulate/run: Receives simulation parameters (file paths, threads, etc.) as a JSON body. It constructs the command-line arguments and launches the simulator process using reproc.  
  * GET /api/stream: An endpoint that uses **Server-Sent Events (SSE)** to stream the simulator's console output directly to the browser in real-time.

#### **Frontend**

* **index.html**: The main UI layout, including sections for mode selection, file configuration, execution monitoring, and results.  
* **style.css**: Defines the visual appearance for a clean, research-focused interface.  
* **app.js**: Handles all client-side logic:  
  * Fetching file lists from the /api/files/scan endpoint to populate selectors.  
  * Sending the simulation configuration to /api/simulate/run.  
  * Using the EventSource API to connect to /api/stream and display the live console output.

# **Step-by-Step Implementation Plan**

This plan breaks down the development into manageable milestones.

### **Milestone 1: Core Infrastructure & Basic Web Server (Days 1-2)** ✅

**Goal:** Establish the foundation by creating the new UI server executable and serving a basic webpage.

1. **Update Root CMakeLists.txt:** ✅  
   * ✅ Add FetchContent commands to download cpp-httplib and reproc.  
   * ✅ Use add\_subdirectory() to include a new ui\_server directory.  
2. **Create the UI Server Project:** ✅  
   * ✅ Create a new directory: ui\_server.  
   * ✅ Inside ui\_server, create ui\_server/CMakeLists.txt to define the tank\_simulator\_ui executable and link it against httplib.  
3. **Implement the Basic Server (ui\_server/ui\_main.cpp):** ✅  
   * ✅ Write the initial C++ code to start a httplib server.  
   * ✅ Implement the GET / endpoint to serve a styled HTML page with "Hello, Simulator\!" message  
   * ✅ Added health check endpoint at /health  
4. **Build and Verify:** ✅  
   * ✅ Run cmake and make from your build directory with -DENABLE_UI=ON flag.  
   * ✅ Ensure the new tank\_simulator\_ui executable is created.  
   * ✅ Run it and verify that you can see the styled page in your browser at http://localhost:8080.

**Implementation Notes:**
- Used cpp-httplib v0.14.3 for web server functionality
- Temporarily removed reproc dependency for initial milestone to avoid compilation conflicts
- Created a styled HTML response with gradient background and professional appearance
- Server includes proper error handling and command-line port configuration
- Health check endpoint returns JSON status for monitoring

**Testing Instructions:**
1. Build with UI enabled: `cd build && cmake -DENABLE_UI=ON .. && make tank_simulator_ui`
2. Run server: `./tank_simulator_ui` (defaults to port 8080)
3. Test in browser: Navigate to http://localhost:8080
4. Test health endpoint: `curl http://localhost:8080/health`
5. Stop server: Press Ctrl+C

### **Milestone 2: Simulator Execution & Real-time Streaming (Days 3-4)** ✅

**Goal:** Launch the simulator from the UI and see its output live in the browser.

1. **Implement Process Execution:** ✅  
   * ✅ In ui\_main.cpp, create the POST /api/simulate/run endpoint.  
   * ✅ Used simplified popen() approach instead of reproc for better stability - eliminated threading race conditions.  
   * ✅ Hardcoded simulator_318835816_211314471 command with proper -basic flag and library paths.  
2. **Implement Real-time Output Streaming:** ✅  
   * ✅ Created GET /api/output endpoint for polling-based output retrieval (more stable than SSE).  
   * ✅ Process execution captures both stdout and stderr with proper error handling.  
   * ✅ Output is cached and made available via polling API for real-time display.  
3. **Update Frontend for Interactivity:** ✅  
   * ✅ Created ui\_server/resources/index.html with modern glassmorphism design and gradient background.  
   * ✅ Created ui\_server/resources/style.css with responsive layout and professional styling.  
   * ✅ Created ui\_server/resources/app.js with simulation control and real-time output polling.  
   * ✅ Added "Start Simulation" and "Stop Simulation" buttons with proper state management.  
   * ✅ Implemented polling-based output updates with auto-scroll and manual controls.

**Implementation Notes:**
- Replaced complex ProcessManager (threading + pipes) with simple popen() approach for stability
- Multiple simulations work reliably without server crashes
- Real-time output streaming via polling (500ms intervals) proves more stable than SSE
- Modern UI with status indicators, auto-scroll, and clear output functionality
- All static resources properly served with correct content types

**Testing Results:**
- ✅ Single simulation execution works correctly
- ✅ Multiple consecutive simulations work without server restart
- ✅ Real-time output streaming displays correctly
- ✅ No memory leaks or resource conflicts observed
- ✅ Server remains stable across multiple simulation runs

### **Milestone 3: Dynamic Configuration & File Discovery (Days 5-6)**

**Goal:** Allow the user to configure a simulation run by selecting files from the filesystem.

1. **Implement File Scanning Endpoint:**  
   * In ui\_main.cpp, create the GET /api/files/scan endpoint.  
   * This endpoint will take a directory path as a query parameter.  
   * Use C++ \<filesystem\> to list the contents of the directory and return them as a JSON array.  
2. **Build the Configuration UI:**  
   * In index.html, add the full UI layout with sections for "Comparative" and "Competition" modes.  
   * Add \<select\> dropdowns or input fields for game maps, game managers, and algorithms.  
3. **Connect Frontend to File Scanning:**  
   * In app.js, on page load, make calls to /api/files/scan for default directories (e.g., ./test\_libraries/common/game\_maps).  
   * Use the returned JSON to dynamically populate the \<select\> dropdowns.  
4. **Update Simulation Execution:**  
   * Modify the "Start Simulation" logic in app.js to gather the selected file paths from the UI.  
   * Send these paths in the JSON body of the POST request to /api/simulate/run.  
   * Update the backend to dynamically build the simulator command from the received JSON.

### **Milestone 4: Polish and Final Touches (Days 7-8)**

**Goal:** Refine the UI, add error handling, and ensure the tool is robust and user-friendly.

1. **Implement Results Dashboard:**  
   * Use the file-watching logic from your original plan or a simple "rescan" button that calls the /api/files/scan endpoint on the output directories.  
   * Display detected result files in the "Results Dashboard" section of the UI.  
   * Add download links for each result file.  
2. **Add Error Handling:**  
   * Provide clear visual feedback in the UI if the simulator exits with an error code.  
   * Handle cases where file paths are invalid or directories cannot be scanned.  
3. **Refine CSS and Layout:**  
   * Improve the styling in style.css to create a polished, professional look.  
   * Ensure the layout is responsive and works well on different screen sizes.  
4. **Finalize Build Integration:**  
   * Add a custom CMake target (e.g., make run\_ui) to build and launch the tank\_simulator\_ui executable for convenience.s