import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# Load the dataset from a CSV file
df = pd.read_csv('/content/benchData.csv')

# Create a swarmplot with different colors for each cluster
sns.swarmplot(x='benchmark', y='crossing', hue='benchmark', data=df, size=3)

# Add labels and title
plt.ylabel('Number of crossings')
sns.despine(left=True, bottom=True)
plt.legend().remove()
plt.xlabel('')

plt.savefig('/content/plot.png', dpi=300)

# Display the plot
plt.show()