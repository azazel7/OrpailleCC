#include "reservoir_sampling.hpp"

template<class feature_type, int feature_count, unsigned int sample_size, class func>
class KNN{
	class DataPoint{
		// The array that stores the features.
		feature_type values[feature_count];

		public:
		/**
		 * Empty constructor.
		 */
		DataPoint(){
		}
		/**
		 * Constructor base on the array of features.
		 * @param features A feature_type array of size feature_count.
		 */
		DataPoint(feature_type const* features){
			for(int i = 0; i < feature_count; ++i)
				values[i] = features[i];
		}
		/**
		 * Constructor that initialize the structure array with a single value.
		 * @param initial_value The value to set the array with.
		 */
		DataPoint(feature_type initial_value){
			for(int i = 0; i < feature_count; ++i)
				values[i] = initial_value;
		}
		/**
		 * A const operator to access the sample like an array.
		 * @param i the index to access.
		 */
		feature_type const& operator[](int const i) const{
			return values[i];
		}
		/**
		 * Operator to access the sample like an array.
		 * @param i the index to access.
		 */
		feature_type& operator[](int const i) {
			return values[i];
		}
		/**
		 * Operator of assignation
		 * @param d The DataPoint to copy.
		 */
		void operator=(DataPoint const& d){
			for(int i = 0; i < feature_count; ++i)
				values[i] = d.values[i];
		}
		/**
		 * Operator of assignation for pointer of feature_type.
		 * @param f An array of size feature_count.
		 */
		void operator=(feature_type const* f){
			for(int i = 0; i < feature_count; ++i)
				values[i] = f[i];
		}
		/**
		 * Return a constant pointer to the array stored.
		 */
		feature_type const* data(void) const{
			return values;
		}
	};
	ReservoirSampling<DataPoint, sample_size, func> reservoir;
	int labels[sample_size];
	unsigned int k;

	public:
		KNN(unsigned int k){
			this->k = k;
		}
		bool train(feature_type const* features, int const label){
			DataPoint data(features);
			int const idx = reservoir.sample_index();
			if(idx >= 0){
				reservoir[idx] = features;
				labels[idx] = label;
			}
			return true;
		}
		int predict(feature_type const* features){
			double smallest_distances[k];
			int smallest_indices[k];
			int longest = 0;

			if(reservoir.count() <= k){
				for(int i = 0; i < k; ++i)
					smallest_indices[i] = i;

			}
			else{
				//Initialize the first k element
				for(int i = 0; i < k; ++i){
					smallest_distances[i] = func::distance(features, reservoir[i].data(), feature_count);
					smallest_indices[i] = i;
					if(smallest_distances[longest] < smallest_distances[i])
						longest = i;
				}
				//Look for a datapoint for which distance is smaller than the longest distance of the smallest distances.
				//TODO Complexity is O(nk), but could be improve to O(n log k) with a heap
				for(int i = k; i < reservoir.count(); ++i){
					double dist = func::distance(features, reservoir[i].data(), feature_count);
					//If we found a distance that is smaller than the longest of the smallest, we replace the longest, then we recompute the new longest.
					if(dist < smallest_distances[longest]){
						smallest_distances[longest] = dist;
						smallest_indices[longest] = i;
						for(int j = 0; j < k; ++j)
							if(smallest_distances[longest] < smallest_distances[j])
								longest = j;
					}
				}
			}
				//TODO then do the majority vote

			return 0;
		}
};
