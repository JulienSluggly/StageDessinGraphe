import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# Load the dataset from a CSV file
df = pd.read_csv('/content/benchData.csv')

# Create a swarmplot with different colors for each cluster
sns.swarmplot(x='benchmark', y='crossing', hue='benchmark', data=df, size=5)

# Add labels and title
plt.ylabel('Number of crossings')
sns.despine(left=True, bottom=True)
plt.legend().remove()
plt.xlabel('')

# Set the y-axis limits
y_min = -10  # Minimum crossing value
y_max = 200  # Maximum crossing value
plt.ylim(y_min, y_max)

plt.grid(which='minor', axis='y', linestyle='solid', color='gray', alpha=0.2)
plt.grid(which='major', axis='y', linestyle='solid', color='gray', alpha=0.5)

# Remove tick marks from the y-axis
plt.tick_params(axis='y', length=0)

plt.savefig('/content/plot.png', dpi=300)

# Display the plot
plt.show()