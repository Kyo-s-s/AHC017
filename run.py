import subprocess
import pipes
import multiprocessing
import sys

CASE = 100
TL = 1000

def execute_case(seed):
    input_file_path = f'tools/in/{seed:04}.txt'
    output_file_path = f'tools/out/{seed:04}.txt'
    with open(input_file_path) as fin:
        with open(output_file_path, 'w') as fout:
            subprocess.run(['./a.out'], stdin=fin, stdout=fout, timeout=TL)
            pipefile = f'tools/memo/pipefile_{seed:04}'
            with pipes.Template().open(pipefile, 'w') as p:
                subprocess.run(['tools/target/release/vis', input_file_path, output_file_path], stdout=p, timeout=TL)
            output = open(pipefile).read()
            assert output 
    return seed, output

def progress(count):
    sys.stdout.write("\033[2K\033[G")
    print(f'{count}/{CASE}', end='', flush=True)


def main():
    scores = []
    count = 0
    with multiprocessing.Pool(max(1, multiprocessing.cpu_count()-2)) as pool:
        for seed, score in pool.imap_unordered(execute_case, range(CASE)):
            # print(count%10, end='', flush=True)
            progress(count)
            try:
                scores.append((int(score.split()[2]), f'{seed:04}'))
            except ValueError:
                print(seed, "ValueError", flush=True)
                print(score, flush=True)
                exit()
            except IndexError:
                print(seed, "IndexError", flush=True)
                print(f"error: {score}", flush=True)
                exit()
            count += 1
    print()
    scores.sort()
    total = sum([s[0] for s in scores])
    ave = total / CASE
    print(f'total: {total}')
    print(f'max: {scores[-1]}')
    print(f'ave: {ave}')
    print(f'min: {scores[0]}')


if __name__ == '__main__':
    main()