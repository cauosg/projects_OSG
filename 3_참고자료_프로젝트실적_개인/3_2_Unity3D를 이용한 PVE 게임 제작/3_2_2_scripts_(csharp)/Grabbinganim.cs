//쥐어잡는 손이 땅위로 올라오면 애니메이션을 실행합니다

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Grabbinganim : MonoBehaviour {
    public float animheight = 0.0f;
    Animator anim;
	// Use this for initialization
	void Start () {
		anim = GetComponent<Animator>();
    }
	
	// Update is called once per frame
	void Update () {
        if (transform.position.y > animheight)
            anim.Play("handrockgrab1");
	}
}
