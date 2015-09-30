def make_bilby_eight_pack_def(tube_gap, tube_width, start_x=0):
    """
    seven tube gaps
    eight tubes
    """
    for i in range(8):
        start_x += tube_width
        print '<location  x=\"%f\" name=\"tube%i\" />' % (i*0.0084, i+1)
        start_x += tube_gap

def make_bilby_panel_def(tube_gap, tube_width, additional_offsets=[0]*4):
    eight_pack_width = (tube_width*8) + (tube_gap * 7)
    start = 5 * eight_pack_width / 2
    offset = 0-start
    for i in range(5):
        print '<location  x=\"%f\" name=\"eight_pack%i\" />' % (offset, i+1) 
        offset += eight_pack_width  
        if i < len(additional_offsets):
            offset += additional_offsets[i]


tube_gap = 0.0004
tube_width = 0.008
additional_offsets_east = [0.001, 0.0008, 0.001, 0.001] 
additional_offsets_west = [0.001, 0.0008, 0.0005, 0.0014] 
print 'eight pack layout'
make_bilby_eight_pack_def(tube_gap, tube_width)
print 'ideal panel'
make_bilby_panel_def(tube_gap, tube_width)
print 'east panel (left)'
make_bilby_panel_def(tube_gap, tube_width, additional_offsets_east)
print 'west panel (right)'
make_bilby_panel_def(tube_gap, tube_width, additional_offsets_west)


    

