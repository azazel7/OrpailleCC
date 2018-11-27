
template<int epsilon, int max_value=32767, int min_value=-32768, int time_unit_difference=1>
class LTC{
	struct data_point{
		int timestamp;
		int value;
	};
	data_point last_transmit_point, UL, LL, new_ul, new_ll, last_data_point;
	int counter = 0; //Counter make sure the algorithm does not fail for the first 3 values

	template<typename K>
	static K min(K const a, K const b){
		if(a > b)
			return b;
		return a;
	}
	template<typename K>
	static K max(K const a, K const b){
		if(a < b)
			return b;
		return a;
	}
	void set_ul_and_ll(void){
		UL = new_ul;
		LL = new_ll;
	}
	void update(int const timestamp, int const value){
		new_ul.timestamp = timestamp;
		new_ul.value = min(value + epsilon, max_value);
		new_ll.timestamp = timestamp;
		new_ll.value = max(value - epsilon, min_value);
	}
	bool need_transmit(void){
		double old_up_deriva = (double)(UL.value - last_transmit_point.value) / (UL.timestamp - last_transmit_point.timestamp) / time_unit_difference;
		double old_low_deriva = (double)(LL.value - last_transmit_point.value) / (LL.timestamp - last_transmit_point.timestamp) / time_unit_difference;
		double new_up_deriva = (double)(new_ul.value - last_transmit_point.value) / (new_ul.timestamp - last_transmit_point.timestamp) / time_unit_difference;
		double new_low_deriva = (double)(new_ll.value - last_transmit_point.value) / (new_ll.timestamp - last_transmit_point.timestamp) / time_unit_difference;

		if(old_up_deriva < new_low_deriva || old_low_deriva > new_up_deriva)
			return true;

		if(old_up_deriva < new_up_deriva) {
			new_ul.value = (new_ul.timestamp - UL.timestamp) / time_unit_difference * old_up_deriva + UL.value;
		}

		if(old_low_deriva > new_low_deriva) {
			new_ll.value = (new_ll.timestamp - LL.timestamp) / time_unit_difference * old_low_deriva + LL.value;
		}
		return false;
	}
	public:
	LTC(){
	}
	void get_value_to_transmit(int& timestamp, int& value){
		//Set the return value
		timestamp = last_transmit_point.timestamp;	
		value = last_transmit_point.value;	

		//Update the last_transmit_point
		int tmp_param = (UL.value + LL.value) / 2;
		last_transmit_point.timestamp = last_data_point.timestamp - 1;
		last_transmit_point.value = tmp_param;

		counter = 1;
		add(last_data_point.timestamp, last_data_point.value);
	}
	int add(int const timestamp, int const value) {
		last_data_point.value = value;
		last_data_point.timestamp = timestamp;
		if(counter == 0){
			last_transmit_point.timestamp = timestamp;
			last_transmit_point.value = value;
			counter = 1;
			return false; //Nothing to compute because not enough values
		}
		update(timestamp, value);
		if(counter == 1){
			set_ul_and_ll();
			counter = 2;
			return false; //Nothing to compute because there is still not enough values
		}
		bool n = need_transmit();	
		if(!n)
			set_ul_and_ll();

		return n;
	}
};