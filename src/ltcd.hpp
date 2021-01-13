#define MAX(x,y) (x)>=(y)?(x):(y);
#define MIN(x,y) (x)<=(y)?(x):(y);
template<class element_type, class timestamp_type, int epsilon, int dimension, int max_data_point=200, int max_value=32767, int min_value=-32768, int time_unit_difference=1>
class LTCD{
	public:
	static int const OUTSIDE = 0;
	static int const INTERSECT = 1;
	static int const INSIDE = -1;
	int counter = 0;

	struct POINT {
		double coordinate[dimension];
	};

	struct DATA_POINT {
		unsigned int timestamp;
		struct POINT data;
	};

	struct CIRCLE {
		struct POINT centre;
		double radius;
	};

	struct LIST_CIRCLE {
		struct CIRCLE * circles;
		int length;
	};

	CIRCLE all_circles[max_data_point];
	int index_all_circles = 0;

	void reInitialList(struct LIST_CIRCLE* list_circle)
	{
		free(list_circle->circles);
		list_circle->circles = NULL;
		list_circle->length = 0;
	}

	void addCircleIntoList(struct LIST_CIRCLE* list_circles, struct CIRCLE new_circle)
	{
		list_circles->length += 1;
		list_circles->circles = (struct CIRCLE *)realloc(list_circles->circles, list_circles->length * sizeof(struct CIRCLE));
		list_circles->circles[list_circles->length - 1] = new_circle;
	}

	// for maximum_x and minimum_x when circle intersect line

	struct LIST_CIRCLE tmp_list_circles = {NULL,0};
	struct POINT centre_point;
	struct POINT tmp_centre;

	int relationOf2Circles(struct CIRCLE pre_circle, struct CIRCLE new_circle)
	{
		double square_dis=0;
		int i;
		for (i = 0; i < dimension; i++) {
			square_dis += (pre_circle.centre.coordinate[i] - new_circle.centre.coordinate[i])
						  *(pre_circle.centre.coordinate[i] - new_circle.centre.coordinate[i]);
		}
		if(square_dis > (pre_circle.radius + new_circle.radius)*(pre_circle.radius + new_circle.radius))
			return OUTSIDE;
		else if (square_dis > (pre_circle.radius - new_circle.radius)*(pre_circle.radius- new_circle.radius))
			return INTERSECT;
		else return INSIDE;
	}

	// !at first, make sure that circle do not inside of every circle in the list_circle.
	// !It means circle have intersection with anyone circle in the list_circle

	int recursive(double left, double right, int param_index) {
		if(param_index ==0)
		{
			tmp_centre.coordinate[param_index] = (left+right)/2;
			return  1;
		}

		int h_index=0, l_index=0;
		double mid;
		double high, low, max, min, square_dis=0, square_dis_to_line=0;
		high = INFINITY;
		low = -INFINITY;
		int i,j;
		while(left < right)
		{
			mid = (left+right)/2;

			for(i=0; i<tmp_list_circles.length;i++)
			{

				square_dis_to_line = (mid-tmp_list_circles.circles[i].centre.coordinate[param_index]) * (mid-tmp_list_circles.circles[i].centre.coordinate[param_index]);
				for(j = param_index+1; j<dimension; j++)
				{
					square_dis_to_line += (tmp_centre.coordinate[j] - tmp_list_circles.circles[i].centre.coordinate[j]) * (tmp_centre.coordinate[j] - tmp_list_circles.circles[i].centre.coordinate[j]);
				}

				square_dis = (tmp_list_circles.circles[i].radius * tmp_list_circles.circles[i].radius)
							 - square_dis_to_line;

				max = tmp_list_circles.circles[i].centre.coordinate[param_index-1] + sqrt(square_dis);
				min  = tmp_list_circles.circles[i].centre.coordinate[param_index-1] - sqrt(square_dis);

				if(i==0 || max < high) {high = max; h_index = i;}
				if(i==0 || min > low) {low = min; l_index = i;}
			}

			if(high >= low)
			{
				tmp_centre.coordinate[param_index] = mid;
				if(recursive(low, high, param_index-1)) {
					tmp_centre.coordinate[param_index] = mid;
					return 1;
				}
			}

			square_dis = 0;
			for(i=0; i<dimension; i++)
			{
				square_dis += (tmp_list_circles.circles[h_index].centre.coordinate[i] - tmp_list_circles.circles[l_index].centre.coordinate[i])
							  *(tmp_list_circles.circles[h_index].centre.coordinate[i] - tmp_list_circles.circles[l_index].centre.coordinate[i]);
			}

			// x0 = x1 + (x2-x1)*(r1^2-r2^2+dis^2)/2dis^2
			double dis_AE = (tmp_list_circles.circles[l_index].radius*tmp_list_circles.circles[l_index].radius)
							- (tmp_list_circles.circles[h_index].radius*tmp_list_circles.circles[h_index].radius)
							+ square_dis;

			double intersect_point = tmp_list_circles.circles[l_index].centre.coordinate[param_index]
									 + (tmp_list_circles.circles[h_index].centre.coordinate[param_index] - tmp_list_circles.circles[l_index].centre.coordinate[param_index])
									   * dis_AE / (2*square_dis);

			if(intersect_point < mid)
				right = mid - 1;
			else left = mid + 1;
		}
		return 0;
	}

	int isThereIntersection(struct LIST_CIRCLE* list_circle, struct CIRCLE new_circle) {
		reInitialList(&tmp_list_circles);
		if (list_circle->length == 0) {
			addCircleIntoList(&tmp_list_circles, new_circle);
			centre_point = new_circle.centre;
			return 1;
		}
		int param_index = dimension - 1;

		double left = new_circle.centre.coordinate[param_index] - new_circle.radius;
		double right = new_circle.centre.coordinate[param_index] + new_circle.radius;

		int i;
		for (i = 0; i < list_circle->length; i++) {
			switch (relationOf2Circles(list_circle->circles[i], new_circle)) {
				case OUTSIDE:
					return 0;
				case INTERSECT:
					left = MAX(left,
							(list_circle->circles[i].centre.coordinate[param_index] - list_circle->circles[i].radius));
					right = MIN(right,
							(list_circle->circles[i].centre.coordinate[param_index] + list_circle->circles[i].radius));
					addCircleIntoList(&tmp_list_circles, list_circle->circles[i]);
					break;
				default:
					break;
			}
		}

		addCircleIntoList(&tmp_list_circles, new_circle);

		if (left > right)
			return 0;

		if(recursive(left, right, param_index) == 1){
			centre_point = tmp_centre;
			return 1;
		}else{
			return 0;
		}
	}

	DATA_POINT base_data;
	DATA_POINT coming_data;
	CIRCLE corresponse_circle;
	LIST_CIRCLE all_circles = {NULL, 0};

	void updateBasePoint()
	{
		int i;
		for(i=0; i<dimension; i++)
		{
			base_data.data.coordinate[i] += (centre_point.coordinate[i] - base_data.data.coordinate[i])
				*(coming_data.timestamp-1 -base_data.timestamp);
		}
		base_data.timestamp = coming_data.timestamp-1;
	}

	void updateCorCircle()
	{
		double ln = coming_data.timestamp - base_data.timestamp;
		int i;
		for(i=0; i<dimension; i++)
		{
			corresponse_circle.centre.coordinate[i] = base_data.data.coordinate[i]
				+ (coming_data.data.coordinate[i] - base_data.data.coordinate[i])/ln;
		}
		corresponse_circle.radius = epsilon/ln;
	}

	int add(timestamp_type const timestamp, element_type const coming_data[dimension]) {
        /* do initial when first and second data coming */
        if(counter == 0) {
			base_data.timestamp = timestamp;
			for(int i = 0; i < dimension; ++i)
				base_data.data.coordinate[i] = coming_data[i];
            return 0;
        }

        updateCorCircle();

        if(!isThereIntersection(&all_circles, corresponse_circle) || (timestamp - base_data.timestamp) > max_data_point) {

            //for (fprintf(f_out, "%u", base_data.timestamp),i = 0; i<DIMENSION_WITHOUT_TIMESTAMP; i++)
                //fprintf(f_out, ",%f", base_data.data.coordinate[i]);
            //fprintf(f_out, "\n");

            // update basePoint
            updateBasePoint();

            // reset all_circle list
            reInitialList(&all_circles);

            updateCorCircle();
            addCircleIntoList(&all_circles, corresponse_circle);
            centre_point = corresponse_circle.centre;
        }
        else{
            reInitialList(&all_circles);
            all_circles.length = tmp_list_circles.length;
            all_circles.circles = (struct CIRCLE *)malloc(sizeof(struct CIRCLE) * tmp_list_circles.length);
            for (int i = 0;  i< tmp_list_circles.length; i++) {
                all_circles.circles[i] = tmp_list_circles.circles[i];
            }
        }

	}

	void get_value_to_transmit(timestamp_type& timestamp, element_type value[dimension]){
	}
};
