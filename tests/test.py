import os
import subprocess as sp
import random
import math

successful = 0
total = 0

def test(fn, inputs, expecteds, func=lambda i,e:i==bytes(e, "UTF-8"), threaded=False):
    global successful
    global total
    print(fn)
    tests_num, successful_ = len(expecteds), 0
    
    for inp, exp in zip(inputs, expecteds):
        command = [binary_path, "-t" if threaded else "", tests_folder + "/" + fn]
        res = sp.run(command, stdout=sp.PIPE, input=bytes(inp, "UTF-8")).stdout
        outcome = func(res, exp)
        if outcome: successful_ += 1
        print("\tInput:{} \t--->\tResult:{}".format(inp, res), end="\t")
        print(outcome, end="")
        if (not outcome): print("\tExpected: {}".format(exp))
        else:             print("")
        
    print()
    print("\tTests passed:{}/{} - {}%\n".format(successful_, tests_num, 100 * successful_ / tests_num))
    successful += successful_
    total += tests_num

## ----------------------------------------------------------------------------
NUM_TESTS = 25

where_am_i   = os.path.abspath(__file__)
tests_folder = "/".join(where_am_i.split("/")[:-1])
binary_path  = "/".join(tests_folder.split("/")[:-1]) + "/bff93"

endspace = lambda l: list(map(lambda s: s+" ", l))

for fn in sorted(os.listdir(tests_folder)):
    if fn[-4:] != "bf93":
        continue

    if fn == "01-get-and-print-int.bf93":
        inputs = [str(random.randint(0, 255)) for _ in range(NUM_TESTS)]
        outputs = endspace(inputs)
        test(fn, inputs, outputs)
    elif fn == "02-hello-world.bf93":
        inputs  = [str(random.randint(0, 255))]
        outputs = ["Hello, World!!\n"]
        test(fn, inputs, outputs)
    elif fn == "03-random.bf93":
        inputs  = [random.randint(0, 250) for _ in range(NUM_TESTS)]
        outputs = [set([x+1,x+2,x+3]) for x in inputs]
        inputs = map(str, inputs)
        
        func    = lambda res,exp: int(res) in exp
        test(fn, inputs, outputs, func)
    elif fn == "04-moving.bf93":
        test(fn, [""], ["9 "])
    elif fn == "05-factorial.bf93":
        inputs  = range(0,10)
        outputs = endspace(list(map(lambda x: str(math.factorial(x) % 256), inputs)))
        inputs  = [str(x) for x in range(10)]
        test(fn, inputs, outputs)
    elif fn == "07-fibonacci.bf93":
        outputs = [1,1]
        for i in range(2,13): outputs.append(outputs[-1] + outputs[-2])
        outputs = endspace(list(map(str, outputs)))
        inputs  = list(map(str, range(0,13)))
        test(fn, inputs, outputs)
    elif fn == "08-parenthesis-matching.bf93":
        inputs  = list(map(lambda s: s + "\n", ["", "[", "[]", "[[[][[[]]]]]", "[[[[[[[[[[[[[[[[[[[[][[[[[[[[[[[[[[[[[]][[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]][[[]]]]]]]]]]]]]]]]]]]][]]]]]]]]]]]]]]]"]))
        outputs = list(map(lambda s: s + "\n", ["OK", "NOT OK", "OK", "OK", "NOT OK"]))
        test(fn, inputs, outputs)
    elif fn == "10-digital-root.bf93":
        def digital_root(x):
            persistance, length = 0, len(str(x))
            while (length != 1):
                #print(x)
                x = sum(int(n) for n in str(x))
                length = len(str(x))
                persistance += 1
            return [x, persistance]
            
        inputs  = [random.randint(0, 999999999) for _ in range(NUM_TESTS)]
        outputs = list(map(digital_root, inputs))
        inputs  = list(map(str, inputs))

        def extract_func(stdout, expected):
            stdout = stdout.decode()
            out_str = stdout.replace("Enter number: ","").replace("\nDigital root: ","").replace("Additive Persistence: ","").split("\n")
            out = list(map(int, out_str[:-1]))
            return expected == out

        test(fn, inputs, outputs, extract_func)
    elif fn == "11-multithread.bf93":
        inputs = [""]
        outputs = ["3 4 5 6 "]
        test(fn, inputs, outputs, threaded=True)
    else:
        print(fn)
        print("\tNo test aviable.")
        print()

print("Total: {}/{} - {}%".format(successful, total, 100 * successful / total))
