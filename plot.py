import pandas as pd
import matplotlib.pyplot as plt

# === choose one dataset ===
df = pd.read_csv("stride_exp2.csv")     # or stride_exp2.csv

plt.figure(figsize=(7,5))
for col in df.columns[1:]:
    plt.plot(df["time"], df[col], marker="o", label=col)

plt.xlabel("Time (quanta)")
plt.ylabel("Cumulative Quanta (ticks)")
plt.title("Stride Scheduling (8:4:2:1 Ticket Allocation)")
plt.legend(title="Process")
plt.grid(True, linestyle="--", alpha=0.5)

plt.tight_layout()
plt.show()
