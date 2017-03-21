def lsum(a, b):
    c=[0,0,0]
    c[0] = a[0] + b[0]
    c[1] = a[1] + b[1]
    c[2] = a[2] + b[2]
    return c

l = [1,0,0]
r = [0,1,0]
path = []
while True:
    print((l, r))
    c = input()
    if c == "l":
        path.append((l, r, "l"))
        r=lsum(l,r)
        l[2]+=1
    elif c == "r":
        path.append((l, r, "r"))
        t = lsum(r,l)
        l = r
        l[2] += 1
        r = t
    elif c == "u":
        if len(path) == 0:
            continue
        l = path[-1][0]
        r = path[-1][1]
        path.pop()
    else:
        pass
