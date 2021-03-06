.. algorithm::

.. summary::

.. alias::

.. properties::

Description
-----------

This algorithm loads all monitors found in a NeXus file into a single
:ref:`Workspace2D <Workspace2D>` (if there is no event data or if 
MonitorsAsEvents is false) or into an 
:ref:`EventWorkspace <EventWorkspace>` (if event monitor data is
found).  The algorithm assumes that all of the monitors are histograms
and have the same bin boundaries. **NOTE:** The entry is assumed to be
in SNS or ISIS format, so the loader is currently not generically
applicable.

This version (v2) fixes a bug in the first version and now returns a
group workspace when invoked from Python with a multiperiod input
workspace. As a side-effect of the fix, the contained individual
workspaces for each of the periods are named slightly differently.

Usage
-----

.. include:: ../usagedata-note.txt

.. testcode:: Ex

    ws = LoadNexusMonitors("CNCS_7860_event.nxs")
    # CNCS has 3 monitors
    print "Number of monitors =", ws.getNumberHistograms()

Output:

.. testoutput:: Ex

    Number of monitors = 3

.. categories::

.. sourcelink::
