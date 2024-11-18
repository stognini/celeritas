.. Copyright 2024 UT-Battelle, LLC, and other Celeritas developers.
.. See the doc/COPYRIGHT file for details.
.. SPDX-License-Identifier: CC-BY-4.0

.. _api_mucf_physics:

*****************************
Muon-catalyzed fusion physics
*****************************

Muon-catalyzed fusion physics (muCF) in Celeritas is implemented based on a
combination of standard Geant4 physics classes (available in the standard Geant4
source code) and physics developed by independent third parties.

.. todo:: Add references for Kevin Lynch, NK Labs, Sridhar Tripathy, and possibly others

Processes and models
====================

The following table summarizes the muCF processes and models implemented in
Celeritas.

.. only:: html

   .. table:: muCF physics processes and models available in Celeritas.

      +----------------+---------------------------+---------------------------+--------------------------------------------------------+-------------------+
      | **Particle**   | **Processes**             |  **Models**               | **Celeritas Implementation**                           | **Applicability** |
      +----------------+---------------------------+---------------------------+--------------------------------------------------------+-------------------+
      | :math:`\mu^-`  | MuMinusAtomCaptureProcess | DTMuMinusAtomCaptureModel | :cpp:class:`celeritas::DTMuMinusAtomCaptureInteractor` | At rest           |
      +----------------+---------------------------+---------------------------+--------------------------------------------------------+-------------------+

.. only:: latex

   .. raw:: latex

      \begin{table}[h]
          \caption{Particle decays available in Celeritas.}
          \begin{threeparttable}
          \begin{tabular}{| l | l | l | l | l |}
            \hline
            \textbf{Particle} & \textbf{Processes} & \textbf{Models} & \textbf{Celeritas Implementation} & \textbf{Applicability} \\
            \hline
            $\mu^-$           & MuMinusAtomCaptureProcess & DTMuMinusAtomCaptureModel & \texttt{\scriptsize celeritas::DTMuMinusAtomCaptureInteractor} & At rest \\
            \hline
          \end{tabular}
          \end{threeparttable}
      \end{table}

Deuteron or triton muonic atom capture
--------------------------------------

.. doxygenclass:: celeritas::DTMuMinusAtomCaptureInteractor
