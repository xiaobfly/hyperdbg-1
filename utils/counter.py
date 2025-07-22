import os

def CountLines(start, lines=0, header=True, begin_start=None):
    if header:
        print('{:>10} |{:>10} | {:<20}'.format('ADDED', 'TOTAL', 'FILE'))
        print('{:->11}|{:->11}|{:->20}'.format('', '', ''))

    for thing in os.listdir(start):
        thing = os.path.join(start, thing)
        if os.path.isfile(thing):
            if ('\\dependencies\\zydis' not in thing and '\\dependencies\\ia32-doc' not in thing and '\\build\\bin' not in thing) and (thing.endswith('.c') or thing.endswith('.h') or thing.endswith('.cpp') or thing.endswith('.asm') or thing.endswith('.py') or thing.endswith('.cs') or thing.endswith('.scala') or thing.endswith('.tcl') or thing.endswith('.scala') or thing.endswith('.sh') or thing.endswith('.config') or thing.endswith('.ds') or thing.endswith('.hds') or thing.endswith('.hex') or thing.endswith('.txt') or thing.endswith('.md') or thing.endswith('.bat') or thing.endswith('.v') or thing.endswith('.sv') or thing.endswith('.vhd')  or thing.endswith('.xdc')):
            
                with open(thing, 'r', encoding="latin-1") as f:
                    newlines = f.readlines()
                    newlines = len(newlines)
                    lines += newlines

                    if begin_start is not None:
                        reldir_of_thing = '.' + thing.replace(begin_start, '')
                    else:
                        reldir_of_thing = '.' + thing.replace(start, '')

                    print('{:>10} |{:>10} | {:<20}'.format(
                            newlines, lines, reldir_of_thing))


    for thing in os.listdir(start):
        thing = os.path.join(start, thing)
        if os.path.isdir(thing):
            lines = CountLines(thing, lines, header=False, begin_start=start)

    return lines

##
## This is fun script, I wrote to see that  
## HyperDbg contains how many lines of code...
##
CountLines(r'..\\.')
