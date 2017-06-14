import tensorflow as tf, sys
import glob, os
import time
from collections import deque

class Classifier:
    def use_deque_stream(self, segments, one_shot):
        # Loads label file, strips off carriage return
        self.label_lines = [line.rstrip() for line
                           in tf.gfile.GFile("./retrained_labels.txt")]

        # Unpersists graph from file
        with tf.gfile.FastGFile("./retrained_graph.pb", 'rb') as f:
            graph_def = tf.GraphDef()
            graph_def.ParseFromString(f.read())
            _ = tf.import_graph_def(graph_def, name='')

        with tf.Session() as sess:
            while True:
                if len(segments) == 0:
                    if one_shot:
                        break;

                    print "No segments available to classify."
                    time.sleep(1)
                    continue

                segment_path = segments.pop()
                print "Processing: " + segment_path

                # Read in the segment_data
                segment_data = tf.gfile.FastGFile(segment_path, 'rb').read()

                # Feed segment_data as input to the graph and get first
                # prediction
                softmax_tensor = sess.graph.get_tensor_by_name('final_result:0')

                predictions = sess.run(softmax_tensor, \
                         {'DecodeJpeg/contents:0': segment_data})

                # Sort to show labels of first prediction in order of confidence
                top_k = predictions[0].argsort()[-len(predictions[0]):][::-1]

                for node_id in top_k:
                    human_string = self.label_lines[node_id]
                    score = predictions[0][node_id]
                    print('%s (score = %.5f)' % (human_string, score))

def main():
    if len(sys.argv) > 1:
        classifier = Classifier()

        segments_to_classify = deque()

        for i in range(1, len(sys.argv)):
            segments_to_classify.append(sys.argv[i])
        print segments_to_classify

        classifier.use_deque_stream(segments_to_classify, True)

if __name__ == "__main__":
    main()
