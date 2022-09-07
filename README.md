# ParticleSystem
Optimized C++ Particle System
- Optimized a simulated system of 25,000 particles for class competition.
- Used Intel SIMD instruction set to enhance matrix and vector math libraries.
- Improved memory usage and data caching by reworking bloated data structures.
- Implemented a heap-like data structure to hold active and inactive particles and improve speeds through particle recycling.
- Used 'Load-in-Place' buffer for quicker initialization of data in particle structure.
- Diagnosed highly active methods and loops and reworked to remove costly operations/incorporate SIMD math.
- Achieved ~10x overall speed boost, fastest system in 25+ student class.
