import torch
from transformers import AutoTokenizer, AutoModel
import numpy as np
from sklearn.linear_model import Ridge
import ctypes
import os

print("Loading NLP model (all-MiniLM-L6-v2)...")
tokenizer = AutoTokenizer.from_pretrained('sentence-transformers/all-MiniLM-L6-v2')
model = AutoModel.from_pretrained('sentence-transformers/all-MiniLM-L6-v2')

def get_embedding(text):
    inputs = tokenizer(text, return_tensors='pt', padding=True, truncation=True)
    with torch.no_grad():
        outputs = model(**inputs)
    # mean pooling
    embeddings = outputs.last_hidden_state.mean(dim=1).squeeze().numpy()
    return embeddings

visual_concepts = {
    "apple":  [1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
    "car":    [0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
    "dog":    [0.0, 0.0, 1.0, 0.0, 0.0, 0.0],
    "house":  [0.0, 0.0, 0.0, 1.0, 0.0, 0.0],
    "water":  [0.0, 0.0, 0.0, 0.0, 1.0, 0.0],
    "sword":  [0.0, 0.0, 0.0, 0.0, 0.0, 1.0],
}

print("Computing NLP embeddings for base concepts...")
X_train = []
y_train = []
for word, vis_vec in visual_concepts.items():
    X_train.append(get_embedding(word))
    y_train.append(vis_vec)

X_train = np.array(X_train)
y_train = np.array(y_train)

print("Training NLP-to-Visual Cross-Modal Mapper (Ridge Regression)...")
mapper = Ridge(alpha=0.1)
mapper.fit(X_train, y_train)

query_words = ["fruit", "vehicle", "puppy", "building", "ocean", "blade", "banana", "truck", "cat"]
print(f"Computing NLP embeddings for queries: {query_words}...")

X_queries = []
for word in query_words:
    X_queries.append(get_embedding(word))
X_queries = np.array(X_queries)

y_pred = mapper.predict(X_queries)

# Normalize the predictions so similarity computation works well
norms = np.linalg.norm(y_pred, axis=1, keepdims=True)
y_pred = y_pred / (norms + 1e-9)

print("\nInitializing MetalBagEngine via ctypes...")
lib_path = os.path.join(os.getcwd(), 'libmetalbag.dylib')
lib = ctypes.CDLL(lib_path)

lib.create_engine.restype = ctypes.c_void_p
lib.destroy_engine.argtypes = [ctypes.c_void_p]
lib.find_closest.argtypes = [
    ctypes.c_void_p, 
    ctypes.POINTER(ctypes.c_float), 
    ctypes.c_int, 
    ctypes.POINTER(ctypes.c_float), 
    ctypes.POINTER(ctypes.c_float)
]
lib.find_closest.restype = ctypes.c_int

engine = lib.create_engine()

num_concepts = len(visual_concepts)
memory_flat = y_train.flatten().astype(np.float32)
memory_ptr = memory_flat.ctypes.data_as(ctypes.POINTER(ctypes.c_float))

concept_names = list(visual_concepts.keys())

print("=== Cross-Modal Association Prototype ===")
print("Base visual concepts loaded into MetalBagEngine:")
for i, name in enumerate(concept_names):
    print(f"  [{i}] {name} -> {visual_concepts[name]}")

print("\nTesting NLP -> Visual mapping and MetalBagEngine retrieval:")
for i, query_word in enumerate(query_words):
    query_vec = y_pred[i].astype(np.float32)
    query_ptr = query_vec.ctypes.data_as(ctypes.POINTER(ctypes.c_float))
    
    sim = ctypes.c_float(0)
    best_id = lib.find_closest(engine, memory_ptr, num_concepts, query_ptr, ctypes.byref(sim))
    
    best_match = concept_names[best_id]
    print(f"Query '{query_word}' -> retrieved visual concept: '{best_match}' (similarity: {sim.value:.4f})")

lib.destroy_engine(engine)
print("\nDone. Prototype working.")
