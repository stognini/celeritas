.. Copyright Celeritas contributors: see top-level COPYRIGHT file for details
.. SPDX-License-Identifier: CC-BY-4.0

.. _api_g4_interface:

Geant4 interface
================

The ``accel`` directory contains components exclusive to coupling Celeritas
with Geant4 for user-oriented integration. A simple interface for multithreaded
or serial applications is demonstrated in :ref:`example_geant`, and the more
advanced implementation can be inspected in the :ref:`celer-g4` app.


.. _api_accel_high_level:

High-level interface
--------------------

The :cpp:class:`celeritas::SimpleOffload` class is an extremely easy-to-use
interface for
offloading tracks to Celeritas in a multithreaded or serial application. The
class names correspond to user actions and ``ActionInitialization``. It
requires a few app-owned pieces such as :cpp:class:`celeritas::SharedParams`
and :cpp:class:`celeritas::LocalTransporter` to be owned by
the calling application; the options described below must also be set up and
provided.

.. doxygenclass:: celeritas::SimpleOffload
   :members:
   :no-link:

The :cpp:class:`SetupOptionsMessenger` can be instantiated with a reference to
a global :cpp:class:`SetupOptions` instance in order to provide a Geant4 "UI"
macro interface to an app's Celeritas options.

.. doxygenclass:: celeritas::SetupOptionsMessenger

Celeritas setup
---------------

The setup options help translate the Geant4 physics and problem setup to
Celeritas. They are also necessary to set up the GPU offloading
characteristics. Future versions of Celeritas will automate more of these
settings.

By default, sensitive detectors are automatically mapped from Geant4 to
Celeritas using the ``enabled`` option of
:cpp:struct:`celeritas::SDSetupOptions`. If no SDs are present (e.g., in a test
problem, or one which has only a "stepping manager" which is not presently
compatible with Celeritas), the Celeritas setup will fail with an error like:

.. code-block:: none

   *** G4Exception : celer0001
         issued by : accel/detail/HitManager.cc:210
   Celeritas runtime error: no G4 sensitive detectors are defined: set `SetupOptions.sd.enabled` to `false` if this is expected
   *** Fatal Exception *** core dump ***


.. doxygenstruct:: celeritas::SetupOptions
   :members:
   :no-link:

.. doxygenstruct:: celeritas::SDSetupOptions
   :members:
   :no-link:

.. doxygenfunction:: celeritas::FindVolumes

.. doxygenclass:: celeritas::UniformAlongStepFactory

.. doxygenclass:: celeritas::RZMapFieldAlongStepFactory

Detailed interface
------------------

These classes are usually integrated into UserActions. The ``SimpleOffload``
interface above hides the complexity of these classes, or for more complex
applications you can choose to use these classes directly instead of it.

.. doxygenclass:: celeritas::SharedParams
   :members:
   :no-link:

.. doxygenclass:: celeritas::LocalTransporter
   :members:
   :no-link:

Interface utilities
-------------------

.. doxygenfunction:: celeritas::MakeMTLogger

.. doxygenclass:: celeritas::ExceptionConverter

.. doxygenstruct:: celeritas::AlongStepFactoryInput

.. doxygenclass:: celeritas::AlongStepFactoryInterface


Classes usable by Geant4
------------------------

These utilities are based on Celeritas data structures and capabilities but are
written to be usable both by the ``celer-g4`` app and potential other users.

.. doxygenclass:: celeritas::GeantSimpleCalo

.. doxygenclass:: celeritas::HepMC3PrimaryGenerator

.. doxygenclass:: celeritas::RZMapMagneticField


Low-level Celeritas integration
-------------------------------

This subsection contains details of importing Geant4 data into Celeritas.

Geant4 geometry utilities
^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfunction:: celeritas::load_geant_geometry
.. doxygenfunction:: celeritas::find_geant_volumes

.. doxygenclass:: celeritas::g4vg::Converter

Geant4 physics interfaces
^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenclass:: celeritas::GeantImporter

.. doxygenclass:: celeritas::GeantSetup


.. _api_geant4_physics_options:

Geant4 physics options
^^^^^^^^^^^^^^^^^^^^^^

.. doxygenstruct:: celeritas::GeantPhysicsOptions
   :members:
   :no-link:


