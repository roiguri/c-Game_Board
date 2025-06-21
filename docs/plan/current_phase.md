1. Move SatelliteView to UserCommon

Move include/satellite_view_impl.h → UserCommon/satellite_view_impl.h
Move src/satellite_view_impl.cpp → UserCommon/satellite_view_impl.cpp
Update include pathso throughout the codebase

2. Move FileLoader to Simulator Directory

Move include/file_loader.h → Simulator/file_loader.h
Move src/file_loader.cpp → Simulator/file_loader.cpp

3. Change FileLoader to Load SatelliteView

Modify FileLoader::loadBoardFile() to return a SatelliteView implementation
Create new method that parses board content into satellite view format
Keep the existing parsing logic but adapt the output format

4. Modify GameManager for AbstractGameManager

Add inheritance from AbstractGameManager
Implement the new run() method signature
Create helper method to convert SatelliteView to GameBoard
Keep existing output creation temporarily

5. Create Basic Simulator

Create minimal Simulator class structure
Set up the foundation for future logic migration
Don't move complex logic yet - just establish the structure