import random
import argparse

from faker import Faker

url_set = []
url_counter = {}
fake = Faker()

parser = argparse.ArgumentParser()
parser.add_argument('n', type=int, help='url set size')
parser.add_argument('m', type=int, help='row size')
parser.add_argument('--path', type=str, help='file path', default='urls.txt')
parser.add_argument('-v', action='store_true', default=False, help='display url count info')

args = parser.parse_args()

for _ in range(args.n):
    url = fake.url()
    url_set.append(url)
    if args.v:
        url_counter[url] = 0

output = open(args.path, "w")

for _ in range(args.m):
    url = random.choice(url_set)
    output.write(url+'\n')
    if args.v:
        url_counter[url] += 1

output.close()

if args.v:
    counter = open("counter.txt", "w")
    for url, cnt in url_counter.items():
        counter.write(f"{url} {cnt}\n")
    counter.close()