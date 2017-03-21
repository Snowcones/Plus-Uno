l = 0
r = 1
path = []
while True:
    print((l, r))
    c = input()
    if c == "l":
        path.append((l, r, "l"))
        r = l+r
        l+=1
    elif c == "r":
        path.append((l, r, "r"))
        if l == 0 and r == 1:
            l = 1
            r = 2
        else:
            t = l+r
            l = r+1
            r = t
    elif c == "u":
        if len(path) == 0:
            pass
        elif path[-1][2] == "l":
            # Undo adding a left
            path.pop()
            nr = l-1
            l = r-l-1
            r = nr
        else:
            # Undo adding a right
            path.pop()
            if l == 1 and r == 2:
                l = 0
                r = 1
            else:
                nr = l-1
                l = l-r+1
                r = nr
    else:
        pass
