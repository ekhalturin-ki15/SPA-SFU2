import pandas as pd
import networkx as nx
import matplotlib.pyplot as plt


LabelFile = '0903010030_op-21Label.csv'
RibFile = '0903010030_op-21Rib.csv'

DataLable = pd.read_csv(LabelFile, sep=';',encoding='cp1251')
DataRib = pd.read_csv(RibFile, sep=';',encoding='cp1251')

"""
for ind in DataLable.index:
    DataRib['Source'] = DataRib['Source'].replace(DataLable['Id'][ind], DataLable['Label'][ind])
    DataRib['Target'] = DataRib['Target'].replace(DataLable['Id'][ind], DataLable['Label'][ind])
"""

G = nx.Graph()

Label = {}
for id, name, w in DataLable.values:
    G.add_node(id)
    Label[id] = name

Edge = DataRib[['Source', 'Target', 'Weight']]
G.add_weighted_edges_from(Edge.values)
#Label = DataLable[['Id', 'Label']].set_index(['Id']).T.to_dict('list')

pos = nx.spring_layout(G, seed=42)
plt.figure(figsize=(30,30))

for edge in G.edges(data='weight'):
    nx.draw_networkx_edges(G, pos, edgelist=[edge], width=edge[2]*40, edge_color='#000011')

nx.draw(G, pos, labels=Label, width=0, with_labels = True, font_size=14, font_weight='bold',
        node_size=DataLable[['Weight']].values * 20, node_color='#00b4d9', font_color='#ee0111')

#plt.show()
plt.savefig('labels.png')