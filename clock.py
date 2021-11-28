from time import sleep, time

if __name__ == "__main__":
    sleep(time() % 1)
    while True:
        print(int(time() // 1))
        sleep(1)

