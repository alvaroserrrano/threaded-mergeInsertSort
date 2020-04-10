A merge-insert sort; divides the original problem into separate collections <=1000.  So, if 10,000 will have 10 parts.  Insert sorts each collection then merges them together.

Parent divides in half and spawns two threads recursively until size <=1000.

Sorts the <= 1000 collection using an insert sort you write.

When two children die the parent merges the two parts into one.

The top program writes the sorted collection to a comma separated file with 10 doubles per line named array.csv or linked.csv
