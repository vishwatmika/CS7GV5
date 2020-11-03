using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WalkIdle : MonoBehaviour
{
    float speed = 4;
    float rotSpeed = 10;
    float gravity = 8;
    float rot = 0;

    Vector3 moveDir = Vector3.zero;
    CharacterController controller;
    Animator animator;
    // Start is called before the first frame update
    void Start()
    {
        controller = GetComponent<CharacterController>();
        animator = GetComponent<Animator>();
    }

    // Update is called once per frame
    void Update()
    {
        if (controller.isGrounded)
        {
            if (Input.GetKey(KeyCode.W))
            {
                moveDir = new Vector3(0, 0, 1);
                moveDir *= speed;
                rot += Input.GetAxis("Horizontal") * rotSpeed * Time.deltaTime;
                transform.eulerAngles = new Vector3(0, rot, 0);
            }
            if (Input.GetKeyUp(KeyCode.W))
            {
                moveDir = new Vector3(0, 0, 0);
            }

            if (Input.GetKey(KeyCode.A))
            {
                moveDir = new Vector3(0, 0, 1);
                moveDir *= speed;
                rot += Input.GetAxis("Horizontal") * rotSpeed * Time.deltaTime;
                transform.eulerAngles = new Vector3(0, rot, 0);
            }
        }

    }
}