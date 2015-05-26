# Input: array of int. 
#   Example: x = [12, 34]
#            longInt(x)
def longInt(a):
    t = 0
    A = []
    for i in range(0, len(a)):
        t = t << 8 | a[i]
    A.append(t)
    # print "t:%d , a[i]:%d" % (t, a[i])
    return t

