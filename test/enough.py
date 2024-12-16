import time
import pandas as pd

df = pd.DataFrame([i for i in range(15000000)])

while True:
    print("Just Enough Still Running...")
    time.sleep(5)

