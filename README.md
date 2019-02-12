# EvacSim
 Emergency evacuation Simulator


The main goal of this project was to develop a piece of software which will simulate the emergency evacuation of a building in case of a fire. People within the building must correctly navigate to an evacuation assembly point from their current position. A Star algorithm was used for people's path-finding as the cheapest and fastest method for computing the shortest path. In order to visualise peoples movement the GLFW OpenGL library was used. The Win32 API was used to create buttons and dialog boxes. 


The final application has a menu, a simulation window and a map editor. 
![Alt text](editor.PNG?raw=true "Map editor")

User can create their own map and run the simulation straight away. The map editor supports fire placement, multiple floor creation, building design and people positioning. 
![Alt text](settings.PNG?raw=true "Settings window")

At the end of the simulation special window appears displaying statistics (e.g. time in seconds).

More: [Detailed report about the project](EvacSimReport.pdf)
