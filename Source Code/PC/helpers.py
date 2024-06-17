def split_data(msg):
    msg_array = msg.split()
    return int(msg_array[0]), int(msg_array[1]), int(msg_array[2]), int(msg_array[3])

def check_ratio(count, data, data30, ratio, name):
    if ratio > 1:
        if data/data30 >= ratio:
            count+= 1
        else:
            count = 0
        if count == 3:
            count = 0
            print(f"unsual {name}")
    else:
        if data/data30 <= ratio:
            count+= 1
        else:
            count = 0
        if count == 3:
            count = 0
            print(f"unsual {name}")


