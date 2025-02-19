# Better Joysticks Plugin for Orbiter
The goal of this plugin is to enable controls to be assigned to Joystick buttons, axes, and HAT switches far beyond what Orbiter's default Joystick handling system can manage.  This project requires the [Input System](https://github.com/TChapman500/InputSystem) library, along with the INI reader component from the [C++ Libraries](https://github.com/TChapman500/CPP-Libraries) repository.  Don't forget to modify the additional include directories and additional library directories (or just drop everything into `Orbitersdk/includes`.

Currently, the solution is set up for Orbiter 2016 but can probably be adapted to other editions of Orbiter.

# Goals
The following are a list of goals for this project.  Note that if you have to implement something via a "hack", you're either doing it wrong, or shouldn't be doing it at all.

## Completion Goals
The "Completion Goals" are what this plugin is required to be able to do in order for this project to be considered complete.  These goals are:
 - Make every action bound to a keyboard be assignable to a button on a joystick if the API allows such an implementation.
 - Make several directional actions assignable to the HAT switch that can logically be assigned to a HAT switch if the API allows such an implementation.
 - Implement many global axis controls through the various API functions available to essentially make the rotation/translation mode switch obsolete.

## Optional Goals
Some "nice to have" features that I would like this plugin to have are:
 - The ability to define custom HAT switches that the device has that does not show up as a HAT (eg: the additional HATs of the Logitech X52 or X56 controllers).
 - The ability for vessel plugins to define and implement their own, vessel-specific controls.
