
import pickle
lifetime_dict = dict()


with open('lifetimes_raw.txt', 'r') as infile:
    lines = [line for line in infile]

    print len(lines)

    # Get lines 0 and 10, every 13 lines

    n = -1
    while n < 135-1:
        n += 1
        # print "{}: {}".format(lines[13*n].rstrip('\n'),lines[13*n + 10].rstrip('\n'))
        run = int(lines[13*n].rstrip('\n'))
        lifetime = int(lines[13*n+10].rstrip('\n'))
        lifetime_dict.update({run : lifetime})

# These runs have lifetimes that are done "by hand" by interpolating between adjacent runs
lifetime_dict[607] = 732
lifetime_dict[622] = 0.5*(734+757)
lifetime_dict[626] = 0.5*(734+757)
lifetime_dict[631] = 789
lifetime_dict[763] = 415
lifetime_dict[827] = 0.5*(580 + 477)
lifetime_dict[828] = 0.5*(580 + 477)
lifetime_dict[843] = 0.5*(559 + 495)
lifetime_dict[844] = 0.5*(559 + 495)

# print lifetime_dict
output = open("lifetimes_data.pkl",'wb')
pickle.dump(lifetime_dict,output)


# Missing electrons:
# print lifetime_dict[827]
# print lifetime_dict[843]
# print lifetime_dict[828]
# print lifetime_dict[622]


# Missing photons
# print lifetime_dict[607]
# print lifetime_dict[827]
# print lifetime_dict[844]
# print lifetime_dict[843]
# print lifetime_dict[622]
# print lifetime_dict[626]
# print lifetime_dict[631]
# print lifetime_dict[763]

