Test Avionics Plugin
====================

This plugins exercise the new graphics and input APIs for both customized stock cockpit devices,
and custom avionics devices.

It installs:

- a "full graphics" override on the pilot-side GNS530 (test in default C172)
- a "overlay graphics" override on the copilot-side GNS430 (test in default C172)
- a "no graphics, only input" override (null graphics callbacks) on the pilot-side 737 CDU
- a new, custom device (with ID "TEST_AVIONICS"), which can be popped up using the command
  `laminar/avionics_test/toggle_popup` (test in modified C172)
