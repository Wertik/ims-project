from click import Argument
import numpy as np
import matplotlib.pyplot as plt
from enum import Enum
from argparse import ArgumentParser
import os

class Stat(Enum):
    AVG_UNTIL_PARKED = 0
    AVG_UNTIL_LEAVE = 1
    AVG_INTER_WAIT = 2
    PERC_LEFT_WITHOUT_PARK = 3

settings: dict[Stat, dict[str, str]] = {
    Stat.AVG_UNTIL_PARKED: {"title": "Průměrný čas pro nalezení parkování", "ylabel": "$[s]$"},
    Stat.AVG_UNTIL_LEAVE: {"title":"Průměrný čas pro odjezd z parkoviště", "ylabel": "$[s]$"},
    Stat.AVG_INTER_WAIT: {"title":"Průměrný čas čekání na křižovatce", "ylabel": "$[s]$"},
    Stat.PERC_LEFT_WITHOUT_PARK: {"title":"Procentuální počet aut, která odjela bez zaparkování", "ylabel": "%"}
}

parser = ArgumentParser(prog="gen_garphs", description="generate graphs from result csv data")
parser.add_argument("-s", "--stat", help="set the statistic we want to generate a graph for", type=int, choices=range(0, len(Stat.__members__)))
parser.add_argument("-f", "--file", help="set the file to take data from, has to be a csv in specific format")
parser.add_argument("-a", "--all", help="generate all the graphs", action="store_true")
args = parser.parse_args()

# Load data from CSV, make a plot
def save_graph(data, res_path: str, stat: Stat):
    x = data[np.arange(data.shape[0]),0:1]
    data = data[np.arange(data.shape[0]),1:]

    fig, ax = plt.subplots()

    y = data[np.arange(data.shape[0]),stat.value:stat.value + 1]

    ax.plot(x, y)
    ax.set_title(settings[stat]["title"])
    ax.set_xlabel("Počet aut v simulaci")
    ax.set_ylabel(settings[stat]["ylabel"])
    plt.savefig(res_path, format="png")

def gen_graph(file: str, stat: Stat):
    data = np.genfromtxt(file, delimiter=",", skip_header=1)

    with open(file, "r") as f:
        header = f.readline().strip().split(",")

    data = data[1:]

    print(data, header)

    res_path = name + f"_{header[stat.value]}.png"

    save_graph(data, res_path, stat)
    print(f"Saved graph of statistic \"{header[stat.value]}\" from {file} into {res_path}.")

if args.stat == None and args.all == False:
    print("Choose a stat please.")
    parser.print_usage()
    exit(1)

if args.file == None:
    print("Choose a file please.")
    parser.print_usage()
    exit(1)

file_path = os.path.join(os.path.dirname(__file__), args.file)
name = os.path.splitext(os.path.basename(file_path))[0]

print(file_path, name)

if args.all == True:
    for stat in Stat.__members__.items():
        gen_graph(file_path, stat[1])
else:
    gen_graph(file_path, Stat(args.stat))
