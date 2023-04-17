#include <algorithm>
#include <experimental/random>
#include "parallel.h"

using namespace parlay;

using Type = long long;


inline uint64_t myhash64(uint64_t u) {
	  u=(u+0x7ed55d16)+(u<<12);
	  u=(u^0xc761c23c)^(u>>19);
	  u=(u+0x165667b1)+(u<<5);
	  u=(u+0xd3a2646c)^(u<<9);
	  u=(u+0xfd7046c5)+(u<<3);
	  u=(u^0xb55a4f09)^(u>>16);
	  return u;
}




size_t* prefixsum(size_t* nums,size_t low,size_t high){
	
	int n=high-low+1;
	if(n<30){
	size_t* B=new size_t[high+1];
        B[0]=nums[0];
	        for(size_t i=1;i<=high;i++){
			        B[i]=nums[i]+B[i-1];
				        }
		        return B;
	}



	size_t* A=new size_t[n/2];
	size_t* res=new size_t[n];

	res[0]=nums[0];

	if(n>1){
	parallel_for(0,n/2,[&](size_t i){A[i]=nums[2*i]+nums[2*i+1];});
	
	
	size_t* temp=prefixsum(A,low,low+(high-low)/2);
	parallel_for(1,n,[&](size_t i){
		if(i%2==1){res[i]=temp[i/2];}	
		else{res[i]=temp[(i-1)/2]+nums[i];}	
			
			});
	}
	return res;

}

size_t scan_up(size_t* A,size_t* LS,size_t n){
	        if(n==1){return A[0];}
		size_t l,r;
		auto f1=[&](){l=scan_up(A,LS,n/2); };
		auto f2=[&](){r=scan_up(A+(n/2),LS+(n/2),n-(n/2)); };
		par_do(f1,f2);
		LS[(n/2)-1]=l;
		return l+r;
	}

void scan_down(size_t* A, size_t* B,size_t* LS,size_t n,size_t offset){
	        if(n==1){B[0]=offset+A[0];return;}
		auto f1=[&](){scan_down(A,B,LS,n/2,offset); };
		auto f2=[&](){scan_down(A+(n/2),B+(n/2),LS+(n/2),n-(n/2),offset+LS[(n/2)-1]); };
		par_do(f1,f2);
}


void scan(size_t* A,size_t n){
	 if(n<1000){
		   for(size_t i=1;i<n;i++){
			   A[i]+=A[i-1];
												                                         }
			return;
											 }
									 
	size_t* LS=new size_t[n];
	scan_up(A,LS,n);
	scan_down(A,A,LS,n,0);
	delete[] LS;	
}


size_t* myscan(size_t* A,size_t start,size_t end){
	size_t n=end-start+1;
	if(n<50){
		                   for(size_t i=1;i<n;i++){
					                              A[i]+=A[i-1];

								                                            }
	return A;
				                                                                                            }
	size_t* B=new size_t[n/2];
	size_t* out=new size_t[n];

	out[0]=A[0];
	
	if(n>1){
	parallel_for(0,n/2,[&](size_t i){B[i]=A[2*i]+A[2*i+1];});
	
	size_t* C=myscan(B,start,start+(end-start)/2);
	parallel_for(1,n,[&](size_t i){
		if(i%2==1){out[i]=C[i/2];}
		else{out[i]=C[(i-1)/2]+A[i];}});
	delete[] C;
	}

	delete[] B;
	return out;
	}

auto f1=[](Type a,Type b){if(a<b){return 1;}else{return 0;}};
auto f2=[](Type a,Type b){if(a==b){return 1;}else{return 0;}};
auto f3=[](Type a,Type b){if(a>=b){return 1;}else{return 0;}};



void filterless(size_t* less,Type* nums,size_t n,Type x){
	parallel_for(0,n,[&](size_t i){less[i]=f1(nums[i],x);});
	scan(less,n);
	}	
void filterequal(size_t* equal,Type* nums,size_t n,Type x){
	parallel_for(0,n,[&](size_t i){equal[i]=f2(nums[i],x);});
	scan(equal,n);
	}
void filtermore(size_t* greater,Type* nums,size_t n,Type x){
	parallel_for(0,n,[&](size_t i){greater[i]=f3(nums[i],x);});
	scan(greater,n);
	}

struct point{
	size_t x;
	size_t y;
};

struct point partition(Type* nums,size_t n,Type p){
	/* 	
	if(n<50){
	size_t temp=0;
	size_t i=0,j=n-1;
	while(true){
	while(i<=n-1 && nums[i]<=x){i++;if(i>n-1){i=n;}}
	while(j>=0 && nums[j]>=x){j--;if(j<0){j=0;}}
	if(i<j){temp=nums[i];nums[i]=nums[j];nums[j]=temp;i++;j--;continue;}
	else if(i<rand){temp=nums[i];nums[i]=nums[rand];nums[rand]=temp;i++;break;}
	else if(rand<j){temp=nums[j];nums[j]=nums[rand];nums[rand]=temp;j--;break;}
	else break;
	}
	return {i,j};
	}
	
	
	size_t low=0;
	if(n==1){return {0,0};}
	Type* temp=new Type[n];
	size_t* less=new size_t[n];
	//size_t* greater=new size_t[n];
	//size_t* equal=new size_t[n];
	
	
	parallel_for(0,n,[&](size_t i){
			temp[i]=nums[low+i];
			//if(temp[i]<x){less[i]=1;}else{less[i]=0;}
			//if(temp[i]>x){greater[i]=1;}else{greater[i]=0;}
			//if(temp[i]==x){equal[i]=1;}else{equal[i]=0;}
			//less[i]=f1(nums[i],x);
			//equal[i]=f2(nums[i],x);
			//greater[i]=f3(nums[i],x);
			});
	//auto f=[&](Type a,Type b){if(a<b){return 1;}else{return 0;}};
	//auto less=[&](int i)->size_t*{return f(nums[i],x);}; 	
	
	//scan(less,n);
	//scan(greater,n);
	//scan(equal,n);
	
	filterless(less,nums,n,x);
	//filterequal(equal,nums,n,x);
	//filtermore(greater,nums,n,x);
	
	size_t lesser=less[n-1];
	parallel_for(0,n,[&](size_t i){
		//	if(temp[i]<x){
		//	nums[low+less[i]-1]=temp[i];	
		//	}
		//	else if(temp[i]>x){
		//	nums[less[n-1]+equal[n-1] +greater[i]-1]=temp[i];
		//	}
		//	else{
		//	nums[less[n-1] +equal[i]-1]=temp[i];
		//	}
		if(temp[i]<x){nums[low+less[i]-1]=temp[i];}
			});

	filtermore(less,nums,n,x);

	parallel_for(0,n,[&](size_t i){if(temp[i]>=x){nums[lesser+less[i]-1]=temp[i];}
			                        });
	
	
	//size_t equaler=equal[n-1];
	
	delete[] temp;
	delete[] less;
	//delete[] equal;
	//delete[] greater;

	//if(lesser ==low){return {lesser+equaler ,lesser};}
	//else if(lesser==n-1) return {lesser ,lesser-1};
	//else return {lesser+equaler ,lesser -1};
	
	return {lesser,lesser};*/	
		
	

	size_t z=0;
	Type *B=new Type[n];
	size_t* lt=new size_t[n];
	size_t* eq=new size_t[n];
	size_t* rt=new size_t[n];

	parallel_for(0,n,[&](size_t i){
		B[i]=nums[i];
		if(B[i]<p){lt[i]=1;}else{ lt[i]=0;}
		if(B[i]>p){rt[i]=1;}else{ rt[i]=0;}
		if(B[i]==p){eq[i]=1;z++;}else{ eq[i]=0;}
		});
	
	scan(lt,n);
	scan(rt,n);

	size_t x=0+lt[n-1];
	size_t y=x+z;

	parallel_for(0,n,[&](size_t i){
		if(B[i]<p){nums[0+lt[i]-1]=B[i];}
		else if(B[i]>p){nums[y+rt[i]-1]=B[i]; }
		else{parallel_for(0,z,[&](size_t j){nums[x+j]=p; });}
		});

	struct point P;
	        P.x=x-1;
		        P.y=y;



	delete[] B;
	delete[] lt;
	delete[] rt;

	

return P;
}

Type get_pivot(Type* array,size_t start,size_t n){
	Type* arr=new Type[5];
	parallel_for(0,5,[&](size_t i){arr[i]=array[start+myhash64(n)%n];});
	std::sort(arr,arr+5);
	Type x=arr[2];
	delete[] arr;
	return x;
}


void myquicksort(Type* A, size_t n) {
	        if(n<1000000000/24){std::sort(A,A+n);return;}
			

		Type pivot=get_pivot(A,0,n);
		struct point P=partition(A,n,pivot);
		auto f1=[&](){myquicksort(A,(P.x)+1);};
		auto f2=[&](){myquicksort(A+P.y,(n-P.y));};
		par_do(f1,f2);
}
template <class T>

void quicksort(T* A, size_t n) {
	myquicksort(A,n);		
}	
