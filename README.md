# NoUniVBE

DOS TSR to prevent UniVBE loading by faking the original UniVBE behavior
There is absolutely no need for a game running inside dosbox to use the legacy VESA code of UniVBE

# Status:
Mimics UniVBE 5.3a TSR "interfaces" - UniVBE think its already installed, in-game linked UniVBE (compatible to 5.3a) is inhibited

Tested with "Street Racer":
Preloading NoUniVBE gives a VGA refresh rate of 70Hz (default under dosbox) instead of 35Hz

# Todos:
Mimic also Version < 5.3a - for example 4.2, 5.0, 5.1 - they use different detection methods
