.. algorithm::

.. summary::

.. alias::

.. properties::

Description
-----------

Two `TimeSeriesProperty <http://www.mantidproject.org/TimeSeriesProperty>`_ logs are merged together
by the time stamps.

Output
------

A MatrixWorkspace.


Usage
-----

**Example - Merge 2 sample logs with value reset:**

.. testcode:: ExMerge2Logs

  Load(Filename=r'HYS_11092_event.nxs',OutputWorkspace='HYS_11092_event')
  MergeLogs(Workspace='HYS_11092_event',LogName1='Phase1',LogName2='Phase2',MergedLogName='PhaseM12',ResetLogValue='1',LogValue1='1',LogValue2='2')
  ws = mtd["HYS_11092_event"]
  newprop = ws.run().getProperty("PhaseM12")
  prop1 = ws.run().getProperty("Phase1")
  prop2 = ws.run().getProperty("Phase2")
  
  print "Phase 1's size = %d, Phase 2's size = %d, Merged size = %d." % (prop1.size(), prop2.size(), newprop.size())
  for i in xrange(5):
      print "Phase1              Time[%d] = %s." % (i, prop1.nthTime(i))
  for i in xrange(5):
      print "Phase2              Time[%d] = %s." % (i, prop2.nthTime(i))
  for i in xrange(5):
      print "Mixed Phase 1 and 2 Time[%d] = %s." % (i, newprop.nthTime(i))

.. testcleanup:: ExMerge2Logs

  DeleteWorkspace(Workspace='HYS_11092_event')

Output:

.. testoutput:: ExMerge2Logs

  Phase 1's size = 21, Phase 2's size = 20, Merged size = 41.
  Phase1              Time[0] = 2012-08-14T18:55:52.390000000 .
  Phase1              Time[1] = 2012-08-14T18:55:52.406000000 .
  Phase1              Time[2] = 2012-08-14T18:55:55.640000000 .
  Phase1              Time[3] = 2012-08-14T18:55:57.171000137 .
  Phase1              Time[4] = 2012-08-14T18:56:01.546000137 .
  Phase2              Time[0] = 2012-08-14T18:55:52.390000000 .
  Phase2              Time[1] = 2012-08-14T18:55:52.406000000 .
  Phase2              Time[2] = 2012-08-14T18:55:57.171000137 .
  Phase2              Time[3] = 2012-08-14T18:56:01.546000137 .
  Phase2              Time[4] = 2012-08-14T18:56:04.390000000 .
  Mixed Phase 1 and 2 Time[0] = 2012-08-14T18:55:52.390000000 .
  Mixed Phase 1 and 2 Time[1] = 2012-08-14T18:55:52.390000000 .
  Mixed Phase 1 and 2 Time[2] = 2012-08-14T18:55:52.406000000 .
  Mixed Phase 1 and 2 Time[3] = 2012-08-14T18:55:52.406000000 .
  Mixed Phase 1 and 2 Time[4] = 2012-08-14T18:55:55.640000000 .

.. categories::

.. sourcelink::
